#include "testpattern.h"

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , m_pattern_running(false)
  , m_rotation(0.0, 0.0, 0.0)
  , m_ant_pos_index(-1)
{
    m_antenna_positions.append( QVector3D(0.00, 0.35, 0.05) );      //Antenna just behind cockpit cover
    m_antenna_positions.append( QVector3D(0.05, -0.1, 0.0) );       //Antenna on side behind wing
    reset_pattern();
}

void TestPattern::center_color(QColor color, QVector3D rotation)
{
    if(m_pattern_running){
        float yaw = m_rotation.z() + 10.0;
        if(yaw > 360){
            float pitch = m_rotation.x();
            yaw = 0.0;
            pitch += 10.0;
            if(pitch > 90){
                pitch = -90.0;
                if(!set_antenna_pos_to_index(m_ant_pos_index+1)){
                    m_pattern_running = false;
                    emit test_completed();
                }
            }
            m_rotation.setX(pitch);
        }
        m_rotation.setZ(yaw);
        emit set_rotation(m_rotation);
    }
}

void TestPattern::reset_pattern(void)
{
    if(!m_pattern_running){
        emit set_zoom(16.0);
        set_antenna_pos_to_index(0);
        m_rotation = QVector3D(-90.0, 0.0, 0.0);
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
