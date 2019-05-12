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

    void pack(MsgPackStream &s);
    bool loadPathFile(QString filename);

    QList<AntennaPair>      m_antenna_pairs;

    typedef QList<QVector<double>> rxdBms_t;
    rxdBms_t  m_rx_bBms;

    QList<QQuaternion> m_rotations;
    QList<QQuaternion> m_positions;
    QList<QQuaternion> m_pos_rotations;

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

    void calcResults(RadioSimResults* simresults);
};

#endif // RADIOSIMULATION_H
