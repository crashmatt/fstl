#ifndef RADIOSIMULATION_H
#define RADIOSIMULATION_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QMap>
#include <QVector3D>
#include <msgpackstream.h>
#include <QQuaternion>

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

//class RadioSimResult
//{
//public:
//    RadioSimResult(int size, double timestamp);

//    double              m_timestamp;
//    QQuaternion         m_rotation;
//    QVector<double>     m_rx_dBs;
//};


class AntennaPair
{
public:
    AntennaPair(Antenna* ant1, Antenna* ant2, Radio* rad1, Radio* rad2);
    Antenna* m_ant1;
    Antenna* m_ant2;
    Radio*  m_rad1;
    Radio*  m_rad2;

    void pack(MsgPackStream &s);
};

class RadioSimResults
{
public:
    RadioSimResults(Radios* radios);

    QList<AntennaPair>      m_antenna_pairs;

//    QList<RadioSimResult>   m_sim_results;

    QList<double>           m_timestamps;
    QList<QQuaternion>      m_rotations;
    QList<QVector<double>>  m_rx_bBms;

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
    void run();

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

    void rotation_step(QQuaternion& rotation, RotationSegment& segment);

    void make_rotations(RadioSimResults *results);
    void calc_results(RadioSimResults *results);
    static QList<QVector<double>> calc_result_block(const QList<QQuaternion> rotations, RadioSimResults* simresults);
};

#endif // RADIOSIMULATION_H
