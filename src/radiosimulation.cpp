#include "radiosimulation.h"
#include "testpattern.h"
#include "radio.h"
//#include "antennadata.h"
#include <QTime>
#include <QFile>
#include <QQuaternion>

#include <math.h>
#include <msgpack.h>
#include <msgpackstream.h>

RotationSegment::RotationSegment()
    :m_start_time(0.0)
    ,m_end_time(0.0)
    ,m_last_time(0.0)
    ,m_rate(0.0)
    ,m_direction(0.0, 0.0, 0.0)
{
}

RadioSimResult::RadioSimResult(int size, double timestamp)
    : m_timestamp(timestamp)
    , m_rotation()
{
    m_rx_dBs.fill(0.0, size);
}



AntennaPair::AntennaPair(Antenna* ant1, Antenna* ant2)
    :m_ant1(ant1)
    ,m_ant2(ant2)
{
}

void AntennaPair::pack(MsgPackStream &s)
{
    s << m_ant1->m_name;
    s << m_ant2->m_name;
}


RadioSimResults::RadioSimResults(Radios* radios)
{
    makeAntennaPairs(radios);
};


void RadioSimResults::pack(MsgPackStream &s)
{
    s << m_antenna_pairs.size();

    foreach(auto pair, m_antenna_pairs){
        pair.                              pack(s);
    }
}


int RadioSimResults::makeAntennaPairs(Radios* radios)
{
    const auto &rads = radios->m_radios;
    int radcount = rads.size();

    for(int i=0; i<radcount; i++){
        auto rad1 = rads[i];
        for(int j=i+1; j<radcount; j++){
            auto rad2 = rads[j];
            foreach(auto ant1, rad1->m_antennas){
                m_antenna_radio_map[ant1] = rad1;
                foreach (auto ant2, rad2->m_antennas) {
                    m_antenna_radio_map[ant2] = rad2;
                    m_antenna_pairs.append(AntennaPair(ant1, ant2));
                }
            }
        }
    }
    return m_antenna_pairs.size();
}



RadioSimulation::RadioSimulation() : QObject(NULL)
    , m_radios(NULL)
    , m_test_pattern(NULL)
    , m_filename("")
    , m_halt(false)
    , m_step_time(0.01)
    , m_end_time(50.0)
    , m_time(0)
    , m_max_runtime_ms(1000)
{

}


RadioSimulation::RadioSimulation(QObject *parent, Radios* radios, TestPattern* test_pattern, QString filename) : QObject(parent)
    , m_radios(radios)
    , m_test_pattern(test_pattern)
    , m_filename(filename)
    , m_halt(false)
    , m_step_time(0.01)
    , m_end_time(50.0)
    , m_time(0)
    , m_max_runtime_ms(1000)
{

}


RadioSimulation::RadioSimulation(const RadioSimulation& radsim) : QObject(radsim.parent())
  , m_radios(radsim.m_radios)
  , m_test_pattern(radsim.m_test_pattern)
  , m_filename(radsim.m_filename)
  , m_halt(false)
  , m_step_time(radsim.m_step_time)
  , m_end_time(radsim.m_end_time)
  , m_time(0)
  , m_max_runtime_ms(radsim.m_max_runtime_ms)
{
}


RadioSimulation::~RadioSimulation()
{

}

void RadioSimulation::run()
{
    m_time = 0;
//    if (m_radios->length() < 2){
//        qDebug("RadioSimulation does not have enough radios to simulate");
//        return;
//    }
    qDebug("RadioSimulation started");

    QTime start_time = QTime();
    QTime stop_time = start_time;
    stop_time.addMSecs(m_end_time);

//    m_time = 0;
//    ulong steps = (ulong) m_end_time / m_step_time;
//    auto dbg_str  = QString("RadioSimulation has %1 steps to run").arg(steps);
//    qDebug(dbg_str.toLatin1());

    QByteArray bytes;
    MsgPackStream packstream(&bytes, QIODevice::WriteOnly);

    packstream << QString("Radio Simulation Results - MessagePack - V0_1");
    m_radios->pack(packstream);

    auto sim_results = RadioSimResults(m_radios);
    sim_results.pack(packstream);

    make_rotations(&sim_results);
    calc_results(&sim_results);

    unsigned long step = 0;
    for(auto& simresult : sim_results.m_sim_results){
        packstream << (quint64) step << simresult.m_timestamp;

        for(auto& rxdB : simresult.m_rx_dBs){
            packstream << rxdB;
        }
    }

    packstream.setFlushWrites(true);
    auto fname = m_filename + ".pack";
    QFile file(fname);
    if (!file.open(QFile::WriteOnly)){
        qDebug("RadioSimulation failed to open file for writing: ", fname);
        return;
    }
    file.write(bytes);
    file.close();

    qDebug("RadioSimulation complete");
}

void RadioSimulation::make_rotations(RadioSimResults *results)
{
    m_time = 0;
    QTime start_time = QTime();
    QTime stop_time = start_time;
    stop_time.addMSecs(m_end_time);

    m_time = 0;
    ulong steps = (ulong) m_end_time / m_step_time;
    auto &simresults = results->m_sim_results;
    simresults.reserve(steps);

    auto dbg_str  = QString("RadioSimulation has %1 steps to run").arg(steps);
    qDebug(dbg_str.toLatin1());

    QQuaternion rotation;
    RotationSegment segment;

    unsigned long step = 0;
    while(m_time < m_end_time){
        m_time += m_step_time;
        rotation_step(rotation, segment);
        auto result = RadioSimResult(0,m_time);
        result.m_rotation = rotation;
        simresults.append(result);
        step++;
    }
}

void RadioSimulation::calc_results(RadioSimResults *results)
{
    QTime start_time = QTime();
    QTime stop_time = start_time;
    stop_time.addMSecs(m_end_time);

    QQuaternion rotation;

    //Frequency in GHz.  Distance in km
    const double path_fspl_const = 20.0*log10(2.4) + 92.45;

    unsigned long step = 0;
    int pair_count =  results->m_antenna_pairs.size();

    auto &pairs = results->m_antenna_pairs;
    auto &simresults = results->m_sim_results;

    for(auto& simresult : simresults){
        rotation = simresult.m_rotation;
        simresult.m_rx_dBs.clear();

        int index = 0;
        foreach(auto pair, pairs){
            Radio* rad1 = results->m_antenna_radio_map[pair.m_ant1];
            Radio* rad2 = results->m_antenna_radio_map[pair.m_ant2];
            bool rad1fixed = rad1->m_fixed;
            bool rad2fixed = rad2->m_fixed;

            auto rad_vect1 = QVector3D();
            auto rad_vect2 = QVector3D();

            if(rad1fixed){
                rad_vect1 = pair.m_ant1->radiationVector(QQuaternion());
            } else {
                rad_vect1 = pair.m_ant1->radiationVector(rotation);
            }
            if(rad2fixed){
                rad_vect2 = pair.m_ant2->radiationVector(QQuaternion());
            } else {
                rad_vect2 = pair.m_ant2->radiationVector(rotation);
            }

            rad_vect1[0] = fabs(rad_vect1[0]);
            rad_vect1[1] = fabs(rad_vect1[1]);
            rad_vect1[2] = fabs(rad_vect1[2]);
            rad_vect2[0] = fabs(rad_vect2[0]);
            rad_vect2[1] = fabs(rad_vect2[1]);
            rad_vect2[2] = fabs(rad_vect2[2]);
            auto ant_gain = QVector3D::dotProduct(rad_vect1, rad_vect2);

            auto ant_gain_dB = 20.0 * log10(ant_gain + 1E-12);

            auto distvect = rad1->m_pos - rad2->m_pos;
            auto dist_km = distvect.length() * 0.001;
            auto chan_loss = path_fspl_const + 20.0*log10(dist_km);

            auto link_gain = ant_gain_dB - chan_loss;

            simresult.m_rx_dBs.append(link_gain);
            index++;
        }

        if(step%1000 == 0){
            auto now = QTime();
            auto runtime = start_time.msecsTo(now);
            auto step_line = QString("Simulation runtime %1ms step:%2\r\n").arg(runtime).arg(step);
            qDebug(step_line.toLatin1());
            if(now > stop_time){
                qDebug("RadioSimulation timeout");
                break;
            }
        }
        step++;
    }
}


void RadioSimulation::rotation_step(QQuaternion& rotation, RotationSegment& segment)
{
    const auto now = m_time;
    while(segment.m_end_time < now){
        //Complete the remaining rotation for this segment
        double delta_time = segment.m_end_time - segment.m_last_time;
        double delta_angle = delta_time * segment.m_rate;
        //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
        QQuaternion rot = QQuaternion::fromAxisAndAngle(segment.m_direction, delta_angle);
        rotation = rotation * rot;
        segment.m_last_time = segment.m_end_time;

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
        segment.m_rate = rate;

        segment.m_start_time = segment.m_end_time;
        segment.m_end_time += time;

        //New direction
        do{
            double dx = ((double) rand() / (double) RAND_MAX) - 0.5;
            double dy = ((double) rand() / (double) RAND_MAX) - 0.5;
            double dz = ((double) rand() / (double) RAND_MAX) - 0.5;
            QVector3D vect(dx, dy, dz);
            if(vect.length() > 0.1){
                vect.normalize();
                segment.m_direction = vect;
            }
        } while (segment.m_direction.length() < 0.1);
    }
    double delta = now - segment.m_last_time;
    double angle = (double) delta * segment.m_rate;
    //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
    QQuaternion rot = QQuaternion::fromAxisAndAngle(segment.m_direction, angle);
    rotation = rotation * rot;

    segment.m_last_time = now;
}

