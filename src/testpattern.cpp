#include "testpattern.h"
#include "radpatterndata.h"
#include <QQuaternion>
#include <QExplicitlySharedDataPointer>

TestPattern::TestPattern(QObject *parent, RadPatternData* rad_patterns) : QObject(parent)
  , m_pattern_running(false)
  , m_rotation()
  , m_ant_pos_index(-1)
  , m_test_index(-1)
  , m_high_speed(false)
  , m_rad_patterns(rad_patterns)
{
//    //Antenna just behind cockpit cover
//    const QQuaternion cockpit_rot = QQuaternion();
//    auto cp_antenna = new Antenna( QVector3D(0.00, 0.35, 0.05)
//                                , cockpit_rot
//                                , "rad_monopole"
//                                ,"rear_right"
//                                , QColor(0,255,0,120) );
//    m_antennas.append( cp_antenna );

    //Antenna on side behind wing
    const QQuaternion rear_right_rot =
            QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90.0) *
            QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), 135);

    auto rr_antenna = new Antenna( QVector3D(0.05, -0.1, 0.0)
                                , rear_right_rot
                                , "rad_monopole"
                                , "rear_right"
                                , QColor(255,0,0,120));
    m_antennas.append( rr_antenna );

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

    if(index < 0){
        m_test_index = 0;
        m_ant_pos_index = 0;
        set_antenna_pos_to_index(0);
    } else {
        Antenna* antenna = m_antennas[m_ant_pos_index];
        Q_ASSERT(antenna != NULL);
        auto datapoint = new AntennaDataPoint(this, rotation, center_color, color_visibility);
        antenna->m_antenna_data.append(datapoint);
        m_test_index++;
    }

    Antenna* antenna = m_antennas[m_ant_pos_index];
    Q_ASSERT(antenna != NULL);
    RadPatternSet* pattern = antenna->m_rad_pattern.data();
    Q_ASSERT(pattern != NULL);

    if(m_test_index >= pattern->rad_data.size()){
//        if(m_ant_pos_index >= m_antennas.size()-1) {
//            m_ant_pos_index = 0;
//            foreach(data, m_antennas){
//                emit antenna_data(*data, m_antenna_configs[m_ant_pos_index]);
//                m_ant_pos_index++;
//            }
            emit antenna_data(m_antennas[m_ant_pos_index]);

            m_ant_pos_index = -1;
            m_test_index = -1;
            m_pattern_running = false;
            emit test_completed();
            return;
//        } else {
//            set_antenna_pos_to_index(m_ant_pos_index+1);
//            m_test_index = 0;
//        }
    }

    auto radpt = pattern->rad_data[m_test_index];
    Q_ASSERT(radpt != NULL);

    m_rotation = radpt->rot; //* antenna->m_rotation.inverted();
    m_rotation.setVector(m_rotation.vector() * -1.0);
//    m_rotation.setX(-m_rotation.x());
//    m_rotation.setY(-m_rotation.y());
//    m_rotation.setZ(-m_rotation.z());
//    m_rotation.setScalar(-m_rotation.scalar());
//    m_rotation.normalize();

    emit set_rotation(m_rotation, m_test_index);
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

    //Delete any previous antenna data
    foreach(auto antenna, m_antennas){
        antenna->deleteAntennaData();
    }

    emit set_zoom(16.0);
    set_antenna_pos_to_index(0);
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
        //Connect patterns
        foreach(auto antenna, m_antennas){
            auto type = antenna->m_type;
            auto pattern = m_rad_patterns->get_data(type).data();
            if(pattern == NULL) return;
            antenna->m_rad_pattern = pattern;
        }

        m_pattern_running = true;
        reset();
        emit set_view_pos( m_antennas[m_ant_pos_index]->m_pos );
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
    if( (index >= m_antennas.size()) || (index < 0) ){
        m_ant_pos_index = index;
        QVector3D pos = {0.0, 0.0, 0.0};

        emit set_view_pos(pos);

        QString name = "antenna";
        emit set_obj_pos(name, pos);
        return false;
    } else {
        m_ant_pos_index = index;
//        if(!m_pattern_running){
//            Antenna* antenna = m_antennas[m_ant_pos_index];
//            Q_ASSERT(antenna != NULL);
//            auto pattern = antenna->m_rad_pattern.data();
//            if(pattern != NULL){
//                auto rad_name = antenna->m_rad_pattern.data()->set_name;
//                emit set_obj_rotation( rad_name, antenna->m_rotation);
//                emit set_obj_pos( rad_name, antenna->m_pos);
//            }
//        }
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

