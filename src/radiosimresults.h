#ifndef RADIOSIMRESULTS_H
#define RADIOSIMRESULTS_H

#include <QMap>
#include <QList>
#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include <msgpackstream.h>

class Radios;

class RadioSimResults
{
public:
    RadioSimResults(Radios* radios);

    void pack(MsgPackStream &s);
    bool loadPathFile(QString filename);

    typedef QList<QVector<double>> rxdBms_t;
    rxdBms_t  m_rx_bBms;

    QList<QList<QQuaternion>>  m_rotation_sets;
    QList<QVector3D>    m_position_sets;
    QMap<int, QMap<int, QList<QQuaternion>>>  m_pos_rotations;

};

#endif // RADIOSIMRESULTS_H
