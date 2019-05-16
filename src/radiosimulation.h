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
#include <QMap>

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
