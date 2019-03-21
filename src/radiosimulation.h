#ifndef RADIOSIMULATION_H
#define RADIOSIMULATION_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QMap>
#include <QVector3D>

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

class RadioSimResult
{
public:
    RadioSimResult(int size, double timestamp);

    double              m_timestamp;
    QVector<double>     m_rx_dBs;
};


class AntennaPair
{
public:
    AntennaPair(Antenna* ant1, Antenna* ant2);
    Antenna* m_ant1;
    Antenna* m_ant2;
};

class RadioSimResults
{
public:
    RadioSimResults(Radios* radios);

    QList<AntennaPair>      m_antenna_pairs;
    QMap<Antenna*, Radio*>  m_antenna_radio_map;

    QList<RadioSimResult>   m_sim_results;
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
};

#endif // RADIOSIMULATION_H
