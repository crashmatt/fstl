#include "radiosimulation.h"
#include "testpattern.h"
#include "radio.h"
//#include "antennadata.h"
#include <QTime>
#include <QElapsedTimer>
#include <QFile>
#include <QQuaternion>
#include <qtconcurrentmap.h>
#include <qtconcurrentrun.h>
#include <QtConcurrentRun>
#include <QFuture>
#include <QStandardPaths>
#include <QDir>

#include <math.h>
#include <msgpack.h>
#include <msgpackstream.h>

AntennaPair::AntennaPair(Radio* rad1, Radio* rad2, Antenna* ant1, Antenna* ant2)
    :m_ant1(ant1)
    ,m_ant2(ant2)
    ,m_rad1(rad1)
    ,m_rad2(rad2)
{
}

RadioSimResults::RadioSimResults(Radios* radios)

{
    makeAntennaPairs(radios);
}

int RadioSimResults::makeAntennaPairs(Radios* radios)
{
    const auto &rads = radios->m_radios;
    int radcount = rads.size();
    if(radcount < 2) return 0;

    auto rad1 = rads[0];
    auto rad2 = rads[1];
    const auto rad1ant_count = rad1->m_antennas.length();
    const auto rad2ant_count = rad2->m_antennas.length();
    for(int ant1index=0; ant1index < rad1ant_count; ant1index++){
        auto& ant1 = rad1->m_antennas[ant1index];
        for(int ant2index=0; ant2index < rad2ant_count; ant2index++){
            auto& ant2 = rad2->m_antennas[ant2index];
            m_antenna_pairs.append(AntennaPair(rad1, rad2, ant1, ant2));
        }
    }
    return m_antenna_pairs.size();
}



bool RadioSimResults::loadPathFile(QString filename)
{
    QFile loadFile(filename);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file to load.");
        return false;
    }

    QByteArray bytes = loadFile.readAll();
    MsgPackStream packstream(&bytes, QIODevice::ReadOnly);

    double x,y,z,w = 0;

    m_rotations.clear();
    quint32 length;
    packstream >> length;
    m_rotations.reserve(length);
    for(auto i=0; i<length; i++){
        packstream >> w;
        packstream >> x;
        packstream >> y;
        packstream >> z;
        m_rotations.append(QQuaternion(w,x,y,z));
    }

    m_positions.clear();
    packstream >> length;
    m_positions.reserve(length);
    for(auto i=0; i<length; i++){
        packstream >> x;
        packstream >> y;
        packstream >> z;
        m_positions.append(QQuaternion(w,x,y,z));
    }

    m_pos_rotations.clear();
    packstream >> length;
    m_pos_rotations.reserve(length);
    for(auto i=0; i<length; i++){
        packstream >> w;
        packstream >> x;
        packstream >> y;
        packstream >> z;
        m_pos_rotations.append(QQuaternion(w,x,y,z));
    }

    loadFile.close();

    return true;
}



RadioSimulation::RadioSimulation() : QObject(NULL)
    , m_radios(NULL)
    , m_test_pattern(NULL)
    , m_filename("")
    , m_halt(false)
    , m_step_time(0.01)
    , m_end_time(10.0)
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

void RadioSimulation::run(QString filename)
{
    RadioSimResults sim_results(m_radios);

    if(!sim_results.loadPathFile(filename)){
        qDebug() << "Failed to load file : " << filename;
        return;
    }

    qDebug("RadioSimulation started");

    QByteArray bytes;
    MsgPackStream packstream(&bytes, QIODevice::WriteOnly);

    packstream << QString("Radio Simulation Results - MessagePack - V0_2");
    m_radios->pack(packstream);

//    sim_results.pack(packstream);

    calcResults(&sim_results);

    unsigned long step = 0;
    for(auto& rxdBms : sim_results.m_rx_bBms){
        for(auto& rxdBm : rxdBms){
            packstream << rxdBm;
        }
    }

    packstream.setFlushWrites(true);
    auto fname = m_filename + ".pack";
    auto dir = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    auto filepath = dir.filePath(fname);
    QFile file(dir.filePath(filepath));
    if (!file.open(QFile::WriteOnly)){
        qDebug("RadioSimulation failed to open file for writing: ", filepath);
        return;
    }
    file.write(bytes);
    file.close();

    qDebug("RadioSimulation complete");
}




void RadioSimulation::calcResults(RadioSimResults* simresults)
{
    //Frequency in GHz.  Distance in km
    const double path_fspl_const = 20.0*log10(2.4) + 92.45;

    QElapsedTimer timer;
    timer.start();

    int index = 0;

    auto& pairs = simresults->m_antenna_pairs;
    auto& rotations = simresults->m_rotations;
    auto& rx_dBms = simresults->m_rx_bBms;

    foreach(auto rotation, rotations){
        QVector<double> dBms;
        foreach(auto pair, pairs){           
            Radio* rad1 = pair.m_rad1;
            Radio* rad2 = pair.m_rad2;
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

            dBms.append(link_gain);
        }
        rx_dBms.append(dBms);
        index++;
        if( (index % 1000) == 0){
            qDebug() << "Sim result:" << index << " done at:" << timer.elapsed();
        }
    }
}


//void RadioSimulation::rotationStep(QQuaternion& rotation, RotationSegment& segment)
//{
//    const auto now = m_time;
//    while(segment.m_end_time < now){
//        //Complete the remaining rotation for this segment
//        double delta_time = segment.m_end_time - segment.m_last_time;
//        double delta_angle = delta_time * segment.m_rate;
//        //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
//        QQuaternion rot = QQuaternion::fromAxisAndAngle(segment.m_direction, delta_angle);
//        rotation = rotation * rot;
//        segment.m_last_time = segment.m_end_time;

//        double angle_ratio = (double) rand() / (double) RAND_MAX;
//        angle_ratio = pow(angle_ratio, 3);
//        const double angle = angle_ratio * 720.0;

//        const double max_angle_max_time = 6.0;
//        const double max_angle_min_time = 3.0;
//        const double min_angle_max_time = 1.0;
//        const double min_angle_min_time = 0.25;

//        const double min_angle_time_range = min_angle_max_time - min_angle_min_time;
//        const double max_angle_time_range = max_angle_max_time - max_angle_min_time;

//        const double angle_time_range = ((1-angle_ratio)*min_angle_time_range) + (angle_ratio*max_angle_time_range);
//        const double angle_time_offset = ((1-angle_ratio)*min_angle_min_time) + (angle_ratio*max_angle_min_time);

//        const double time_ratio = (double) rand() / (double) RAND_MAX;
//        const double time = (time_ratio * angle_time_range) + angle_time_offset;

//        const double rate = angle / time;
//        segment.m_rate = rate;

//        segment.m_start_time = segment.m_end_time;
//        segment.m_end_time += time;

//        //New direction
//        do{
//            double dx = ((double) rand() / (double) RAND_MAX) - 0.5;
//            double dy = ((double) rand() / (double) RAND_MAX) - 0.5;
//            double dz = ((double) rand() / (double) RAND_MAX) - 0.5;
//            QVector3D vect(dx, dy, dz);
//            if(vect.length() > 0.1){
//                vect.normalize();
//                segment.m_direction = vect;
//            }
//        } while (segment.m_direction.length() < 0.1);
//    }
//    double delta = now - segment.m_last_time;
//    double angle = (double) delta * segment.m_rate;
//    //Rotation axis is a 90deg rotation. Scale it to a 360deg/s
//    QQuaternion rot = QQuaternion::fromAxisAndAngle(segment.m_direction, angle);
//    rotation = rotation * rot;

//    segment.m_last_time = now;
//}

