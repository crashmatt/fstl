#include "testpattern.h"

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , m_pattern_running(false)
  , m_rotation(0.0, 0.0, 0.0)
  , m_ant_pos_index(-1)
  , m_pitch_steps(36)
  , m_pitch_index(0)
  , m_yaw_steps(18)
  , m_yaw_index(0)
  , m_measure_index(0)

{
    m_antenna_positions.append( QVector3D(0.00, 0.35, 0.05) );      //Antenna just behind cockpit cover
    m_antenna_positions.append( QVector3D(0.05, -0.1, 0.0) );       //Antenna on side behind wing
    reset_pattern();
}

void TestPattern::center_color(QColor color, QVector3D rotation)
{
    if(m_pattern_running){
        m_results.append(color);
        m_measure_index++;
        m_pitch_index++;
        if(m_pitch_index >= m_pitch_steps){
            m_pitch_index = 0;
            m_yaw_index++;
            if(m_yaw_index > m_yaw_steps){
                m_yaw_index = 0;
                if(!set_antenna_pos_to_index(m_ant_pos_index+1)){
                    m_pattern_running = false;
                    emit test_completed();
                    return;
                }
            }
        }

        float pitch_ratio = (2.0 * ((float) m_pitch_index / (float) m_pitch_steps)) - 1.0;
        float pitch = pitch_ratio * 180.0;
        float yaw_ratio = (float) m_yaw_index / (float) m_yaw_steps;
        float yaw = (yaw_ratio * 180.0) - 90.0;
        m_rotation.setX(pitch);
        m_rotation.setZ(yaw);
        emit set_rotation(m_rotation);
    }
}

void TestPattern::reset_pattern(void)
{
    if(!m_pattern_running){
        m_measure_index = 0;
        m_results.clear();
        m_results.reserve(m_antenna_positions.size() * m_pitch_steps * (m_yaw_steps+1));
        emit set_zoom(16.0);
        set_antenna_pos_to_index(0);
        m_yaw_index = 0;
        m_pitch_index = 0;
        m_rotation = QVector3D(0.0, 0.0, 0.0);
        emit set_rotation(m_rotation);
    }
}

void TestPattern::start_pattern(void)
{
    if(!m_pattern_running){
        reset_pattern();
        emit set_view_pos( m_antenna_positions[m_ant_pos_index] );
        emit set_rotation(m_rotation);
    }
    m_pattern_running = true;
}

void TestPattern::stop_pattern(void)
{
    m_pattern_running = false;
}


bool TestPattern::set_antenna_pos_to_index(int index)
{
    if(index >= m_antenna_positions.count()){
        return false;
    }
    m_ant_pos_index = index;
    QString name = "antenna";
    QVector3D antenna_offset = m_antenna_positions[m_ant_pos_index];
    emit set_obj_pos(name, antenna_offset);
    emit set_view_pos(antenna_offset);
    return true;
}

void TestPattern::step_antenna_pos(void)
{
    if(!m_pattern_running){
        if(m_ant_pos_index < 0){
            set_antenna_pos_to_index(0);
        } else {
            if(!set_antenna_pos_to_index(m_ant_pos_index+1)){
                set_antenna_pos_to_index(0);
            }
        }
    }
    redraw();
}
