#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include "antennadata.h"
#include <msgpackstream.h>

class Radio : public QObject
{
    Q_OBJECT
public:
    Radio();
    explicit Radio(QObject *parent, QString name, QVector3D pos, bool fixed = true);
    explicit Radio(QObject *parent, QJsonObject &json);
    ~Radio();
    Radio(const Radio&);
    Radio& operator=(const Radio&);

    QString     m_name;
    QVector3D   m_pos;
    QVector<Antenna*> m_antennas;
    QStringList m_objects;
    bool        m_fixed;

    bool add_antenna(Antenna *antenna);
    void add_object(QString obj_name) {m_objects.append(obj_name);};
    void deleteAntennaData();

    void write_config(QJsonObject &json) const;
    void pack(MsgPackStream &s);
    void pack_all(MsgPackStream &s);

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
//    friend MsgPackStream & operator<<(MsgPackStream &os, const Radio& p);
};


//MsgPackStream &operator<<(MsgPackStream &s, const Radio &radio);
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

    void load_config(const QJsonObject &json);
    void write_config(QJsonObject &json) const;
    void pack(MsgPackStream &s);
    void pack_all(MsgPackStream &s);

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
//    friend MsgPackStream & operator<<(MsgPackStream &s, const Radios& radios);
};


QDataStream &operator<<(QDataStream &, const Radios &);
QDataStream &operator>>(QDataStream &, Radios &);
//MsgPackStream &operator<<(MsgPackStream &s, const Radios &);


#endif // RADIO_H
