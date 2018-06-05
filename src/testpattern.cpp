#include "testpattern.h"

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , m_pattern_running(false)
  , m_rotation(0.0, 0.0, 0.0)
  , m_ant_pos_index(-1)
  , m_pitch_segments(18)
  , m_pitch_index(0)
  , m_yaw_segments(10)
  , m_yaw_index(0)

{
//    m_antenna_positions.append( QVector3D(0.0, 0.0, 0.0) );      //On center
    m_antenna_positions.append( QVector3D(0.00, 0.35, 0.05) );      //Antenna just behind cockpit cover
//    m_antenna_positions.append( QVector3D(0.05, -0.1, 0.0) );       //Antenna on side behind wing
    reset_pattern();
}

void TestPattern::antenna_visibility(int index, QVector3D rotation, float center_color, float color_visibility)
{
    if( m_pattern_running ){
        AntennaData* data = m_results[m_ant_pos_index];
        if(index < 0){
            m_pitch_index = 0;
            m_yaw_index = 0;
        } else {
            AntennaDataPoint* datapoint = new AntennaDataPoint(this, rotation);
            datapoint->m_center_color = center_color;
            datapoint->m_color_visibility = color_visibility;
            data->set_antenna_datapoint(datapoint, m_pitch_index, m_yaw_index);

            m_pitch_index++;
            if(m_pitch_index > m_pitch_segments){
                m_pitch_index = 0;
                m_yaw_index++;
                if(m_yaw_index > m_yaw_segments){
                    m_yaw_index = 0;
                    if(!set_antenna_pos_to_index(m_ant_pos_index+1)){
                        foreach(data, m_results){
                            emit antenna_data(*data);
                        }
                        m_pattern_running = false;
                        emit test_completed();
                        return;
                    }
                }
            }
        }

        float pitch_ratio = (2.0 * ((float) m_pitch_index / (float) m_pitch_segments)) - 1.0;
        float pitch = pitch_ratio * 180.0;
        float yaw_ratio = (float) m_yaw_index / (float) m_yaw_segments;
        float yaw = (yaw_ratio * 180.0) - 90.0;
        m_rotation.setX(pitch);
        m_rotation.setZ(yaw);
        emit set_rotation(m_rotation, data->data_index(m_yaw_index, m_pitch_index));
        emit redraw();

    }
}

void TestPattern::reset_pattern(void)
{
    if(!m_pattern_running){
        QString del_pattern("ant_vis*");
        emit delete_object(del_pattern);

        foreach(AntennaData* data, m_results){
            data->deleteLater();
        }
        m_results.clear();

        foreach(QVector3D pos, m_antenna_positions){
            AntennaData* data = new AntennaData(this, pos, m_pitch_segments+1, m_yaw_segments+1, m_results.count());
            m_results.append(data);
        }

        emit set_zoom(16.0);
        set_antenna_pos_to_index(0);
        m_yaw_index = 0;
        m_pitch_index = 0;
        m_rotation = QVector3D(0.0, 0.0, 0.0);
        emit set_rotation(m_rotation, -1);
        emit redraw();
    }
}

void TestPattern::start_pattern(void)
{
    if(!m_pattern_running){
        reset_pattern();
        emit set_view_pos( m_antenna_positions[m_ant_pos_index] );
        emit set_rotation(m_rotation, -1);
        emit redraw();
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
