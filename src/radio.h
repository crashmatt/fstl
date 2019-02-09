#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include "antennadata.h"

class Radio : public QObject
{
    Q_OBJECT
public:
    Radio();
    explicit Radio(QObject *parent, QString name, QVector3D pos);
    ~Radio();
    Radio(const Radio&);
    Radio& operator=(const Radio&);

    QString     m_name;
    QVector3D   m_pos;
    QVector<Antenna*> m_antennas;

    bool add_antenna(Antenna *antenna);

    void write_config(QJsonObject &json) const;

    static const uint RADIO_VERSION = 0xAB01;

signals:
    void antenna_data_update(Radio& radio, Antenna& antenna);
    void radio_data_update(Radio& radio);

public slots:
    void antennaDataChanged(Antenna *antenna);

protected:
    void delete_antennas(void);

private:
    friend QDataStream & operator<<(QDataStream &os, const Radio& p);
    friend QDataStream & operator>>(QDataStream &os, Radio& p);
};


QDataStream &operator<<(QDataStream &, const Radio &);
QDataStream &operator>>(QDataStream &, Radio &);


////////////////////////////////////////////////////////////////////////////////

class Radios : public QObject
{
    Q_OBJECT
public:
    Radios();
    explicit Radios(QObject *parent);
    ~Radios();
    Radios(const Radios&);
    Radios& operator=(const Radios&);

    QList<Radio*> m_radios;

    void delete_radios(void);
    bool add_radio(Radio *radio);

    void write_config(QJsonObject &json) const;

    static const uint RADIOS_VERSION = 0x5A01;

signals:
    void radio_data(Radio* radio, Antenna* antenna);

public slots:
    void antenna_data_changed(Radio &radio, Antenna &antenna);

protected:
    static Radios* s_radios;

private:
    friend QDataStream & operator<<(QDataStream &os, const Radios& p);
    friend QDataStream & operator>>(QDataStream &os, Radios& p);
};


QDataStream &operator<<(QDataStream &, const Radios &);
QDataStream &operator>>(QDataStream &, Radios &);


#endif // RADIO_H
