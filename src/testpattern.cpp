#include "testpattern.h"


AntennaConfig::AntennaConfig(QVector3D pos, QVector3D rot)
  : m_pos(pos)
  , m_rotation(rot)
{

}

TestPattern::TestPattern(QObject *parent) : QObject(parent)
  , m_pattern_running(false)
  , m_rotation(0.0, 0.0, 0.0)
  , m_ant_pos_index(-1)
  , m_pitch_segments(-1)
  , m_pitch_index(0)
  , m_yaw_segments(-1)
  , m_yaw_index(0)
  , m_high_speed(false)
{
//    m_antenna_positions.append( QVector3D(0.0, 0.0, 0.0) );      //On center
    m_antenna_configs.append( AntennaConfig( QVector3D(0.00, 0.35, 0.05),  QVector3D(0.0, 0.0, 0.0) ) );      //Antenna just behind cockpit cover
    m_antenna_configs.append( AntennaConfig( QVector3D(0.05, -0.1, 0.0),   QVector3D(90.0, 135.0, 0.0) ) );       //Antenna on side behind wing
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

        QString vis_pattern = "monopole";
        emit set_object_visible(vis_pattern, false);

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
        m_rotation = QVector3D(0.0, 0.0, 0.0);
        emit set_rotation(m_rotation, -1);
        emit redraw();
    }
}

void TestPattern::start_pattern(void)
{
    if(!m_pattern_running){
        reset_pattern();
        emit set_view_pos( m_antenna_configs[m_ant_pos_index].m_pos );
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
    if(index >= m_antenna_configs.count()){
        m_ant_pos_index = -1;
        QString name = "ant_vis*";
        emit set_object_visible(name, true);
        QVector3D pos = {0.0, 0.0, 0.0};
        emit set_view_pos(pos);
        name = "monopole";
        emit set_obj_rotation(name, pos);
        return false;
    }
    m_ant_pos_index = index;
    QString name = "antenna";
    QVector3D antenna_offset = m_antenna_configs[m_ant_pos_index].m_pos;
    emit set_obj_pos(name, antenna_offset);
    emit set_view_pos(antenna_offset);
    name = "ant_vis*";
    emit set_object_visible(name, false);
    name = QString("ant_vis%1").arg(m_ant_pos_index);
    emit set_object_visible(name, true);
    name = "monopole";
    emit set_obj_rotation(name, m_antenna_configs[m_ant_pos_index].m_rotation);
    emit set_object_visible(name, true);
    return true;
}

void TestPattern::step_antenna_pos(void)
{
    if(!m_pattern_running){
        if(m_ant_pos_index < 0){
            set_antenna_pos_to_index(0);
        } else {
            set_antenna_pos_to_index(m_ant_pos_index+1);
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
