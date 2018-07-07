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
  , m_min_timestep(100)
  , m_max_timestep(1000)
  , m_max_rate(360.0)
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
    if(!m_pattern_running)
        return;

    if(m_rotations_running){
        rotation_step(1/60.0);
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

    QString vis_pattern = "rad*";
    emit set_object_visible(vis_pattern, false);

    emit set_rotation(rotation, -1);
    emit redraw();
}

void rotation_step()
{

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

void TestPattern::start_rotations(const double timeout, const double min_time, const double max_time,const double max_rate)
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

    m_min_timestep = min_time;
    m_max_timestep = max_time;
    m_max_rate = max_rate;
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
        double delta = m_rotation_segment.end_time - m_last_rotation_time;
        double angle = delta * m_rotation_segment.rate;
        //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
        QQuaternion rot = QQuaternion::fromAxisAndAngle(m_rotation_segment.direction, angle*4);
        m_rotation = m_rotation * rot;
        m_last_rotation_time = m_rotation_segment.end_time;
        int timestep;
        do{
            timestep = qrand() * m_max_timestep * (1/(double) RAND_MAX);
        } while(timestep < m_min_timestep);
        m_rotation_segment.start_time = m_rotation_segment.end_time;
        m_rotation_segment.end_time += timestep;

        do{
            double dx = ((double) rand() / (double) RAND_MAX) - 0.5;
            double dy = ((double) rand() / (double) RAND_MAX) - 0.5;
            double dz = ((double) rand() / (double) RAND_MAX) - 0.5;
            QVector3D vect(dx, dy, dz);
            if(vect.length() > 0.1){
                vect.normalize();
                m_rotation_segment.direction = vect;
                m_rotation_segment.rate = (double) rand() * (1.0/RAND_MAX);
                m_rotation_segment.rate *= m_rotation_segment.rate;
                m_rotation_segment.rate *= m_rotation_segment.rate;
                m_rotation_segment.rate *= m_max_rate;
            }
        } while (m_rotation_segment.direction.length() < 0.1);
    }
    double delta = now - m_last_rotation_time;
    double angle = (double) delta * m_rotation_segment.rate;
    //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
    QQuaternion rot = QQuaternion::fromAxisAndAngle(m_rotation_segment.direction, angle*4);
    m_rotation = m_rotation * rot;

    m_last_rotation_time = now;

    emit set_rotation(m_rotation, -1);
    emit redraw();
}

void TestPattern::generate_rotations(const double timeout, const double min_time, const double max_time, const double max_rate)
{
//    double time = 0.0;
//    QTime now;
//    srand(now.msecsSinceStartOfDay());

//    m_rotation_steps.clear();

//    const double max_delta_time = max_time - min_time;
//    auto rot = QQuaternion();
//    while(time < timeout ){
//        const double timestep = ((double) rand() * (max_delta_time) * (1.0/RAND_MAX)) + min_time;
//        double dx = ((double) rand() / (double) RAND_MAX) - 0.5;
//        double dy = ((double) rand() / (double) RAND_MAX) - 0.5;
//        double dz = ((double) rand() / (double) RAND_MAX) - 0.5;
//        QVector3D vect(dx, dy, dz);
//        if(vect.length() > 0.1){
//            vect.normalize();
//            rotation_step step;
//            step.direction = vect;
//            step.timestep = timestep;
//            step.timestamp = timestamp;
//            step.angle = (double) rand() * max_rate * (1.0/RAND_MAX);
//            m_rotation_steps.append(step);
//            time += timestep;
//        }
//    }
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
