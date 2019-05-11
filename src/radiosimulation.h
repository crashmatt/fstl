#ifndef RADIOSIMULATION_H
#define RADIOSIMULATION_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QMap>
#include <QVector3D>
#include <msgpackstream.h>
#include <QQuaternion>
#include <QMutex>

class TestPattern;
class Radios;
class Antenna;
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


class AntennaPair
{
public:
    AntennaPair(Radio* rad1, Radio* rad2, Antenna* ant1, Antenna* ant2);
    Antenna*    m_ant1;
    Antenna*    m_ant2;
    Radio*      m_rad1;
    Radio*      m_rad2;

    void pack(MsgPackStream &s);
};

class RadioSimResults
{
public:
    RadioSimResults(Radios* radios);

    QList<AntennaPair>      m_antenna_pairs;

    QList<double>           m_timestamps;
    QList<QQuaternion>      m_rotations;

    QMutex                  m_mutex;

    typedef QList<QVector<double>> rxdBms_t;
    rxdBms_t  m_rx_bBms;

    void pack(MsgPackStream &s);

protected:
    int makeAntennaPairs(Radios* radios);
};

class RadioSimulation : public QObject
{
    Q_OBJECT
public:
    RadioSimulation();
    explicit RadioSimulation(QObject *parent, Radios* radios, TestPattern* test_pattern, QString filename);
    RadioSimulation(const RadioSimulation&);
    ~RadioSimulation();
    void run(QString filename);

signals:
//    float percentage_done();

public slots:
//    void stop();

private:
    Radios*         m_radios;
    TestPattern*    m_test_pattern;
    QString         m_filename;
    bool            m_halt;
    double          m_step_time;
    double          m_end_time;
    double          m_time;
    unsigned long   m_max_runtime_ms;

    void rotationStep(QQuaternion& rotation, RotationSegment& segment);
    void makeRotations(RadioSimResults *results);
    void calcResults(RadioSimResults *results);
    bool loadPathFile(QString filename);
    static QList<QVector<double>> calcResultBlock(ulong start, ulong end, RadioSimResults* simresults);
};

#endif // RADIOSIMULATION_H
