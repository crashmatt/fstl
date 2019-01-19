#include "testpattern.h"
#include "radpatterndata.h"
#include <QQuaternion>
#include <QExplicitlySharedDataPointer>
#include <random>
#include <QTime>
#include "time.h"

TestPattern::TestPattern(QObject *parent, RadPatternData* rad_patterns) : QObject(parent)
  , m_pattern_running(false)
  , m_ant_pos_index(-1)
  , m_test_index(-1)
  , m_high_speed(false)
  , m_rad_patterns(rad_patterns)
  , m_rotation_segment()
  , m_last_rotation_time(0)
  , m_rotation_timeout(0)
  , m_rotations_running(false)
  , m_rotation()

{
    reset();
}

TestPattern::~TestPattern()
{
    qDeleteAll(m_antennas);
    m_antennas.clear();
}


void TestPattern::antenna_visibility(int index, QQuaternion rotation, float center_color, float color_visibility)
{
    if(!m_pattern_running){
        auto data = QStringList();
        auto angles = rotation.toEulerAngles();
        auto angles_str = QString(" View Rot X:%1  Y:%2  Z:%3").
                arg(angles.x(),4,'F',0,' ').
                arg(angles.y(),4,'F',0,' ').
                arg(angles.z(),4,'F',0,' ');
        data.append(angles_str);

        auto vect = rotation.rotatedVector(QVector3D(0.0, 0.0, -1.0));
        auto vect_str = QString("View Vect X:%1 Y:%2 Z:%3").
                arg(vect.x(),5,'f',2,' ').
                arg(vect.y(),5,'f',2,' ').
                arg(vect.z(),5,'f',2,' ');
        data.append(vect_str);

//        auto ant_nom_quat = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90.0) * rotation.inverted();
//        auto ant_nom_vect = ant_nom_quat.rotatedVector(QVector3D(0.0, 1.0, 0.0));
        auto ant_nom_quat = rotation.inverted();
        auto ant_nom_vect = ant_nom_quat.rotatedVector(QVector3D(0.0, 0.0, 1.0));
        auto ant_nom_vect_str = QString(" Ant Vect X:%1 Y:%2 Z:%3").
                arg(ant_nom_vect.x(),5,'f',2,' ').
                arg(ant_nom_vect.y(),5,'f',2,' ').
                arg(ant_nom_vect.z(),5,'f',2,' ');
        data.append(ant_nom_vect_str);

//        auto ant_nom_rot = ant_nom_quat.toEulerAngles();
        auto ant_nom_rot = ant_nom_quat.toEulerAngles();
        auto ant_nom_rot_str = QString(" Ant Rot X:%1 Y:%2 Z:%3").
                arg(ant_nom_rot.x(),4,'F',0,' ').
                arg(ant_nom_rot.y(),4,'F',0,' ').
                arg(ant_nom_rot.z(),4,'F',0,' ');
        data.append(ant_nom_rot_str);


        foreach(auto antenna, m_antennas){
            auto rad_data = antenna->m_rad_pattern.data();
            if(rad_data == NULL) return;

            auto& ant_data = antenna->m_antenna_data;
            if(ant_data.length() != rad_data->rad_data.length()){
                auto str = QString("%1 has invalid antenna data").arg(antenna->m_name);
                data.append(str);
            } else {
//                auto ant_angle = (rotation * antenna->m_rotation).toEulerAngles();
//                auto ant_angle = antenna->m_rotation.rotatedVector(QVector3D(0.0, 0.0, -1.0));
//                auto ant_ang_str = QString("%1 Vect  X:%2 Y:%3 Z:%4").
//                        arg(antenna->m_name,10).
//                        arg(ant_angle.x(),3,'f',1).
//                        arg(ant_angle.y(),3,'f',1).
//                        arg(ant_angle.z(),3,'f',1);
//                data.append(ant_ang_str);

//                auto ant_quot_rot = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90.0) * antenna->m_rotation.inverted() * rotation;
                auto ant_quot_rot = rotation * antenna->m_rotation.inverted();

                auto ant_delta_angle = ant_quot_rot.toEulerAngles();
                auto ant_dang_str = QString("%1 Delta X:%2 Y:%3 Z:%4").
                        arg(antenna->m_name,10).
                        arg(ant_delta_angle.x(),3,'f',1).
                        arg(ant_delta_angle.y(),3,'f',1).
                        arg(ant_delta_angle.z(),3,'f',1);
                data.append(ant_dang_str);

//                auto ant_face_vect = ant_quot_rot.rotatedVector(QVector3D(0.0, 0.0, -1.0));
//                auto ant_face_vect_str = QString("%1 Vector X:%2 Y:%3 Z:%4").
//                        arg(antenna->m_name,10).
//                        arg(ant_face_vect.x(),3,'f',1).
//                        arg(ant_face_vect.y(),3,'f',1).
//                        arg(ant_face_vect.z(),3,'f',1);
//                data.append(ant_face_vect_str);

//                auto ant_face_vect = antenna->m_rotation.rotatedVector(QVector3D(0.0, -1.0, 0.0));
//                auto ant_face_vect_str = QString("%1 Face vector  X:%2 Y:%3 Z:%4").
//                        arg(antenna->m_name,10).
//                        arg(ant_face_vect.x(),3,'f',1).
//                        arg(ant_face_vect.y(),3,'f',1).
//                        arg(ant_face_vect.z(),3,'f',1);
//                data.append(ant_face_vect_str);

//                auto ant_face_delta_angle = (ant_nom_quat * antenna->m_rotation * rotation.inverted()).toEulerAngles();
//                auto ant_fdang_str = QString("%1 Face Delta X:%2 Y:%3 Z:%4").
//                        arg(antenna->m_name,10).
//                        arg(ant_face_delta_angle.x(),3,'f',1).
//                        arg(ant_face_delta_angle.y(),3,'f',1).
//                        arg(ant_face_delta_angle.z(),3,'f',1);
//                data.append(ant_fdang_str);

                auto nearest = antenna->m_rad_pattern.data()->nearest_point(ant_quot_rot);
                auto ant_nearest_str = QString("%1 Rad Phi:%2 theta:%3 amp:%4").
                        arg(antenna->m_name,10).
                        arg(nearest->phi,3,'f',1).
                        arg(nearest->theta,3,'f',1).
                        arg(nearest->get_amplitude(),4,'f',2);
                data.append(ant_nearest_str);

                auto rad_data_index = antenna->m_rad_pattern.data()->get_index(nearest->phi, nearest->theta);
                auto& rad_data = antenna->m_antenna_data[rad_data_index];
                auto ant_rad_str = QString("%1 Rad ampl:%2").
                        arg(antenna->m_name,10).
                        arg(rad_data.m_visibility,4,'f',2);
                data.append(ant_rad_str);

            }

        }
        emit antenna_debug_text(data);
        return;
    }

    auto data = QStringList();
    emit antenna_debug_text(data);

    if(m_rotations_running){
        rotation_step(1/60.0);
        emit set_rotation(m_rotation, -1);
        emit redraw();
        return;
    }

    if(index < 0){
        m_test_index = 0;
        m_ant_pos_index = 0;
        set_antenna_pos_to_index(0);
    } else {
        Antenna* antenna = m_antennas[m_ant_pos_index];
        Q_ASSERT(antenna != NULL);
        antenna->m_antenna_data.append(AntennaDataPoint(this, rotation, center_color, color_visibility));
        m_test_index++;
    }

    Antenna* antenna = m_antennas[m_ant_pos_index];
    Q_ASSERT(antenna != NULL);
    RadPatternSet* pattern = antenna->m_rad_pattern.data();
    Q_ASSERT(pattern != NULL);

    if(m_test_index >= pattern->rad_data.size()){
        if(m_ant_pos_index >= m_antennas.size()-1) {
            m_ant_pos_index = 0;
            foreach(antenna, m_antennas){
                emit antenna_data(m_antennas[m_ant_pos_index]);
                m_ant_pos_index++;
            }

            m_ant_pos_index = -1;
            m_test_index = -1;
            m_pattern_running = false;
            emit test_completed();
            return;
        } else {
            set_antenna_pos_to_index(m_ant_pos_index+1);
            antenna = m_antennas[m_ant_pos_index];
            Q_ASSERT(antenna != NULL);
            pattern = antenna->m_rad_pattern.data();
            Q_ASSERT(pattern != NULL);
            m_test_index = 0;
        }
    }

    auto radpt = pattern->rad_data[m_test_index];
    Q_ASSERT(radpt != NULL);

    //get rotation and mirror it to look in the opposite direction.
    auto new_rotation =  antenna->m_rotation * radpt->rot;

    emit set_rotation(new_rotation, m_test_index);
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
    del_pattern = "ant_eff*";
    emit delete_object(del_pattern);

    //Delete any previous antenna data
    foreach(auto antenna, m_antennas){
        antenna->deleteAntennaData();
    }

    emit set_zoom(16.0);
    set_antenna_pos_to_index(0);
    auto rotation = QQuaternion();

    QString vis_pattern = "rad_*";
    emit set_object_visible(vis_pattern, false);

    emit set_rotation(rotation, -1);
    emit redraw();
}


void TestPattern::start_pattern(void)
{
    if(!m_pattern_running){
        //Connect patterns
        foreach(auto antenna, m_antennas){
            auto type = antenna->m_type;
            auto pattern = m_rad_patterns->get_data(type).data();
            if(pattern == NULL) return;
            antenna->m_rad_pattern = pattern;
        }

        m_pattern_running = true;
        m_rotations_running = false;
        reset();
        emit set_view_pos( m_antennas[m_ant_pos_index]->m_pos );
        emit redraw();
    }
}

void TestPattern::stop_pattern(void)
{
    m_pattern_running = false;
    m_rotations_running = false;
}


bool TestPattern::set_antenna_pos_to_index(int index)
{
    if( (index >= m_antennas.size()) || (index < 0) ){
        m_ant_pos_index = index;
        QVector3D pos = {0.0, 0.0, 0.0};

        emit set_view_pos(pos);

        QString name = "antenna";
        emit set_obj_pos(name, pos);

        auto rad_rot = QQuaternion();
        QString rad_name = "rad_*";
        emit set_obj_rotation( rad_name, rad_rot);
        emit set_obj_pos( rad_name, pos);

        return false;
    } else {
        m_ant_pos_index = index;
        if(!m_pattern_running){
            Antenna* antenna = m_antennas[m_ant_pos_index];
            Q_ASSERT(antenna != NULL);
            QString rad_name = "rad_*";
            emit set_obj_rotation( rad_name, antenna->m_rotation);
            emit set_obj_pos( rad_name, antenna->m_pos);
        }
    }
    QString name = "antenna";
    QVector3D antenna_offset = m_antennas[m_ant_pos_index]->m_pos;
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
            if(index >= m_antennas.size()) {
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

void TestPattern::start_rotations(const double timeout)
{
    if(m_pattern_running) return;
    m_rotations_running = true;
    m_pattern_running = true;
    m_rotation_timeout = timeout;
    m_last_rotation_time = 0;

    m_rotation_segment.direction = QVector3D(0.0, 0.0, 0.0);
    m_rotation_segment.end_time = 0;
    m_rotation_segment.start_time = 0;
    m_rotation_segment.rate = 0;

    m_rotation = QQuaternion();

    emit set_rotation(m_rotation , -1);
    emit redraw();
}

bool TestPattern::add_antenna(Antenna &antenna)
{
    //Don't add an atenna that already exists
    foreach(auto ant, m_antennas){
        if(ant->m_name == antenna.m_name){
            return false;
        }
    }

    //If no radiation pattern is available for antenna then exit
    auto pattern = m_rad_patterns->get_data(antenna.m_type);
    if(pattern.data() == NULL) return false;

    //Set antenna rad pattern reference
    antenna.m_rad_pattern = pattern;

    auto new_antenna = new Antenna(antenna);
    m_antennas.append(new_antenna);

    //If size of pattern and antenna data matches then announce it
    if(antenna.m_rad_pattern.data()->rad_data.size() == antenna.m_antenna_data.size()){
        antenna_data(new_antenna);
    }
    return true;
}

void TestPattern::delete_antennas(void)
{
    qDeleteAll(m_antennas);
    m_antennas.clear();
    QString del_objs = "ant_vis*";
    emit delete_object(del_objs);
    del_objs = "ant_rad*";
    emit delete_object(del_objs);
}

void TestPattern::rotation_step(double delta_time)
{
    double now = m_last_rotation_time + delta_time;

    while(m_rotation_segment.end_time < now){
        //Complete the remaining rotation for this segment
        double delta_time = m_rotation_segment.end_time - m_last_rotation_time;
        double delta_angle = delta_time * m_rotation_segment.rate;
        //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
        QQuaternion rot = QQuaternion::fromAxisAndAngle(m_rotation_segment.direction, delta_angle);
        m_rotation = m_rotation * rot;
        m_last_rotation_time = m_rotation_segment.end_time;

        double angle_ratio = (double) rand() / (double) RAND_MAX;
        angle_ratio = pow(angle_ratio, 3);
        const double angle = angle_ratio * 720.0;

        const double max_angle_max_time = 6.0;
        const double max_angle_min_time = 3.0;
        const double min_angle_max_time = 1.0;
        const double min_angle_min_time = 0.25;

        const double min_angle_time_range = min_angle_max_time - min_angle_min_time;
        const double max_angle_time_range = max_angle_max_time - max_angle_min_time;

        const double angle_time_range = ((1-angle_ratio)*min_angle_time_range) + (angle_ratio*max_angle_time_range);
        const double angle_time_offset = ((1-angle_ratio)*min_angle_min_time) + (angle_ratio*max_angle_min_time);

        const double time_ratio = (double) rand() / (double) RAND_MAX;
        const double time = (time_ratio * angle_time_range) + angle_time_offset;

        const double rate = angle / time;
        m_rotation_segment.rate = rate;

        m_rotation_segment.start_time = m_rotation_segment.end_time;
        m_rotation_segment.end_time += time;

        //New direction
        do{
            double dx = ((double) rand() / (double) RAND_MAX) - 0.5;
            double dy = ((double) rand() / (double) RAND_MAX) - 0.5;
            double dz = ((double) rand() / (double) RAND_MAX) - 0.5;
            QVector3D vect(dx, dy, dz);
            if(vect.length() > 0.1){
                vect.normalize();
                m_rotation_segment.direction = vect;
            }
        } while (m_rotation_segment.direction.length() < 0.1);
    }
    double delta = now - m_last_rotation_time;
    double angle = (double) delta * m_rotation_segment.rate;
    //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
    QQuaternion rot = QQuaternion::fromAxisAndAngle(m_rotation_segment.direction, angle);
    m_rotation = m_rotation * rot;

    m_last_rotation_time = now;
}

QDataStream &operator<<(QDataStream &out, const TestPattern &pattern)
{
    const int count = pattern.m_antennas.size();    //pattern.antenna_count();
    out << count;

    for(int index=0; index < count; index++){
        out << *pattern.m_antennas[index];
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, TestPattern &pattern)
{
    int count;
    Antenna antenna;

    in >> count;

    for(int index=0; index<count; index++){
        in >> antenna;
        pattern.add_antenna(antenna);
    }
    return in;
}
