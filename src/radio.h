#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include "antennadata.h"


class Radio : public QObject
{
    Q_OBJECT
public:
    explicit Radio(QObject *parent, QString name, QVector3D pos);

    QString     m_name;
    QVector3D   m_pos;
    QVector<Antenna*> m_antennas;

    void add_antenna(Antenna* antenna) { m_antennas.append(antenna);};

signals:

public slots:
};

#endif // RADIO_H
