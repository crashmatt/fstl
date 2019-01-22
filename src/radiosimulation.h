#ifndef RADIOSIMULATION_H
#define RADIOSIMULATION_H

#include <QObject>
#include <QThread>
#include <QVector3D>

class Radio;

class RotationSegment
{
public:
    RotationSegment();

    double      m_start_time;
    double      m_end_time;
    double      m_last_time;
    double      m_rate;
    QVector3D   m_direction;
};


class RadioSimulation : public QThread
{
    Q_OBJECT
public:
    explicit RadioSimulation(QObject *parent, QVector<Radio*> *radios, QString filename);
    ~RadioSimulation();
    void run();

signals:
//    float percentage_done();

public slots:
//    void stop();

private:
    QVector<Radio*> *m_radios;
    QString         m_filename;
    bool            m_halt;
    double          m_step_time;
    double          m_end_time;
    double          m_time;
    unsigned long   m_max_runtime_ms;

    void rotation_step(QQuaternion& rotation, RotationSegment& segment);
};

#endif // RADIOSIMULATION_H
