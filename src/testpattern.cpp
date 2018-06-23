#include "testpattern.h"

#include <QQuaternion>

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , m_pattern_running(false)
  , m_rotation()
  , m_ant_pos_index(-1)
  , m_pitch_segments(-1)
  , m_pitch_index(0)
  , m_yaw_segments(-1)
  , m_yaw_index(0)
  , m_high_speed(false)
{
//    m_antenna_configs.append( AntennaConfig( QVector3D(0.00, 0.35, 0.05)
//                                             ,QVector3D(0.0, 0.0, 0.0)
//                                             ,"rad_monopole"
//                                             ,"cockpit" ) );      //Antenna just behind cockpit cover

    QQuaternion rear_right_rot =
            QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), 90.0) *
            QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), 135);
//    rear_right_rot.normalize();

    m_antenna_configs.append( AntennaConfig( QVector3D(0.05, -0.1, 0.0)
                                             ,rear_right_rot
                                             ,"rad_monopole"
                                             ,"rear_right" ) );       //Antenna on side behind wing
    reset();
}

void TestPattern::antenna_visibility(int index, QQuaternion rotation, float center_color, float color_visibility)
{
    if(!m_pattern_running)
        return;

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
                if(m_ant_pos_index >= m_antenna_configs.size()-1) {
                    m_ant_pos_index = 0;
                    foreach(data, m_results){
                        emit antenna_data(*data, m_antenna_configs[m_ant_pos_index]);
                        m_ant_pos_index++;
                    }
                    m_ant_pos_index = -1;
                    m_pattern_running = false;
                    emit test_completed();
                    return;
                } else {
                    set_antenna_pos_to_index(m_ant_pos_index+1);
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

void TestPattern::reset_pattern(void)
{
    if(!m_pattern_running){
        reset();
    }
}

void TestPattern::reset()
{
    QString del_pattern("ant_vis*");
    emit delete_object(del_pattern);

    foreach(AntennaData* data, m_results){
        data->deleteLater();
    }
    m_results.clear();

    if(m_high_speed){
        m_pitch_segments = 12;
        m_yaw_segments = 6;
    } else {
        m_pitch_segments = 36;
        m_yaw_segments = 18;
    }

    foreach(AntennaConfig config, m_antenna_configs){
        AntennaData* data = new AntennaData(this, config.m_pos, m_pitch_segments+1, m_yaw_segments+1, m_results.count());
        m_results.append(data);
    }

    emit set_zoom(16.0);
    set_antenna_pos_to_index(0);
    m_yaw_index = 0;
    m_pitch_index = 0;
    m_rotation = QQuaternion();

    QString name = "ant_vis*";
    emit set_object_visible(name, false);

    QString vis_pattern = "rad*";
    emit set_object_visible(vis_pattern, false);

    emit set_rotation(m_rotation, -1);
    emit redraw();
}


void TestPattern::start_pattern(void)
{
    if(!m_pattern_running){
        m_pattern_running = true;
        reset();
        emit set_view_pos( m_antenna_configs[m_ant_pos_index].m_pos );
        emit set_rotation(m_rotation, -1);
        emit redraw();
    }
}

void TestPattern::stop_pattern(void)
{
    m_pattern_running = false;
}


bool TestPattern::set_antenna_pos_to_index(int index)
{
    if( (index >= m_antenna_configs.size()) || (index < 0) ){
        m_ant_pos_index = index;
        QVector3D pos = {0.0, 0.0, 0.0};
        auto rot = QQuaternion();
        emit set_view_pos(pos);
        if(!m_pattern_running){
            QString name = "rad_*";
            emit set_obj_rotation( name, rot);
        }
        return false;
    } else {
        m_ant_pos_index = index;
        if(!m_pattern_running){
            AntennaConfig* config = &m_antenna_configs[m_ant_pos_index];
            emit set_obj_rotation( config->m_type, config->m_rotation);
            emit set_obj_pos( config->m_type, config->m_pos);
        }
    }
    QString name = "antenna";
    QVector3D antenna_offset = m_antenna_configs[m_ant_pos_index].m_pos;
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
            int index = m_ant_pos_index + 1;
            if(index >= m_antenna_configs.size()) {
                index = -1;
            }
            set_antenna_pos_to_index(index);
        }
    }
    redraw();
}

void TestPattern::set_speed(bool high_speed)
{
    if(!m_pattern_running){
        m_high_speed = high_speed;
    }
}

//void TestPattern::set_segments(int pitch_segments, int yaw_segments)
//{
//    if(!m_pattern_running){
//        m_pitch_segments = pitch_segments;
//        m_yaw_segments = yaw_segments;
//    }

//}
