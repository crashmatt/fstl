#include "radio.h"

Radio::Radio() : QObject(NULL)
  , m_name("unknown")
  , m_pos(0.0, 0.0, 0.0)
{

}

Radio::Radio(QObject *parent, QString name, QVector3D pos) : QObject(parent)
  , m_name(name)
  , m_pos(pos)
{

}


Radio::Radio(const Radio& radio) : QObject(radio.parent())
    ,m_name(radio.m_name)
    ,m_pos(radio.m_pos)
    ,m_antennas(radio.m_antennas)
{
}

Radio& Radio::operator=(const Radio& radio)
{
    this->setParent(radio.parent());
    m_name = radio.m_name;
    m_pos = radio.m_pos;
    m_antennas = radio.m_antennas;
}


bool Radio::add_antenna(Antenna &antenna)
{
    //Don't add an antenna that already exists
    foreach(auto ant, m_antennas){
        if(ant->m_name == antenna.m_name){
            return false;
        }
    }

    //Get radiation pattern if it exits
    auto pattern = RadPatternData::get_data(antenna.m_type);
    //Set antenna rad pattern reference
    antenna.m_rad_pattern = pattern;

    auto new_antenna = new Antenna(antenna);
    m_antennas.append(new_antenna);
    emit antenna_data_update(new_antenna);

    return true;
}

void Radio::delete_antennas(void)
{
    qDeleteAll(m_antennas);
    m_antennas.clear();
    emit antenna_data_update(NULL);
}


QDataStream &operator<<(QDataStream &out, const Radio &radio)
{
    const int count = radio.m_antennas.size();    //pattern.antenna_count();
    out << count;

    for(int index=0; index < count; index++){
        out << *radio.m_antennas[index];
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, Radio &radio)
{
    int count;
    Antenna antenna;

    in >> count;

    for(int index=0; index<count; index++){
        in >> antenna;
        radio.add_antenna(antenna);
    }
    return in;
}


////////////////////////////////////////////////////////////////////////////////


Radios::Radios() : QObject(NULL)
{
}

Radios::Radios(QObject *parent)  : QObject(parent)
{
}

Radios::~Radios()
{
}

Radios::Radios(const Radios& radios)
{
    foreach(auto radio, radios.m_radios){
        add_radio(radio);
    }
}

Radios::Radios& operator=(const Radios& radios)
{
    foreach(auto radio, radios.m_radios){
        add_radio(radio);
    }
}

void Radios::delete_radios(void)
{
    qDeleteAll(m_radios);
    m_radios.clear();
    emit radio_data(NULL, NULL);
}

bool Radios::add_radio(Radio &radio)
{
    //Don't add an antenna that already exists
    foreach(auto rad, m_radios){
        if(rad->m_name == radio.m_name){
            return false;
        }
    }

    auto new_radio = new Radio(radio);
    m_radios.append(new_radio);
    connect(new_radio, Radio::antenna_data_update, this, Radios::antenna_data_changed);

    emit radio_data(radio, NULL);
    return true;
}

void Radios::antenna_data_changed(Radio &radio, Antenna &antenna)
{
    emit radio_data(&radio, &antenna);
}

QDataStream &operator<<(QDataStream &, const Radios &)
{
}

QDataStream &operator>>(QDataStream &, Radios &)
{
}

