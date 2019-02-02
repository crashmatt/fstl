#include "radiosimulation.h"
#include "testpattern.h"
#include "radio.h"
#include "antennadata.h"
#include <QTime>
#include <QFile>
#include <QQuaternion>

#include <math.h>

RotationSegment::RotationSegment()
    :m_start_time(0.0)
    ,m_end_time(0.0)
    ,m_last_time(0.0)
    ,m_rate(0.0)
    ,m_direction(0.0, 0.0, 0.0)
{
}

RadioSimulation::RadioSimulation(QObject *parent, Radios* radios, TestPattern* test_pattern, QString filename)
    : QThread(parent)
    , m_radios(radios)
    , m_test_pattern(test_pattern)
    , m_filename(filename)
    , m_halt(false)
    , m_step_time(0.001)
    , m_end_time(10.0)
    , m_time(0)
    , m_max_runtime_ms(1000)
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

    m_time = 0;
    ulong steps = (ulong) m_end_time / m_step_time;
    auto dbg_str  = QString("RadioSimulation has %1 steps to run").arg(steps);
    qDebug(dbg_str.toLatin1());

    QFile file(m_filename);
    if (!file.open(QFile::WriteOnly)){
        qDebug("RadioSimulation failed to open file for writing: ", m_filename);
        return;
    }

    QQuaternion rotation;
    RotationSegment segment;

    unsigned long step = 0;
    while(m_time < m_end_time){
        m_time += m_step_time;
        rotation_step(rotation, segment);
        step++;

        auto line = QString("%1,%2").arg(step).arg(m_time);
        file.write(line.toUtf8());

        foreach(auto radio, m_radios->m_radios){
            foreach(auto antenna, radio->m_antennas){
                auto rad_vect = antenna->radiationVector(rotation);
                auto rad_vect_str = QString(",%1,%2,%3").arg(rad_vect.x()).arg(rad_vect.y()).arg(rad_vect.z());
                file.write(rad_vect_str.toUtf8());
            }
        }

        file.write("\r\n");

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
    }
    file.close();

    qDebug("RadioSimulation complete");
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
