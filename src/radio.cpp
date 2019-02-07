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
    foreach(auto ant, radio.m_antennas){
        auto antenna = new Antenna(*ant);
        add_antenna(std::move(antenna));
    }
}

Radio::~Radio()
{
    delete_antennas();
}

Radio& Radio::operator=(const Radio& radio)
{
    this->setParent(radio.parent());
    m_name = radio.m_name;
    m_pos = radio.m_pos;
    m_antennas = radio.m_antennas;
}


bool Radio::add_antenna(Antenna *antenna)
{
    auto new_antenna = std::move(antenna);
    //Don't add an antenna that already exists
    foreach(auto ant, m_antennas){
        if(ant->m_name == new_antenna->m_name){
            delete(new_antenna);
            return false;
        }
    }

    //Get radiation pattern if it exits
    auto rad_pattern_data = RadPatternData::get_instance();
    auto pattern = rad_pattern_data->get_data(new_antenna->m_type);
    //Set antenna rad pattern reference
    new_antenna->m_rad_pattern = pattern;

    m_antennas.append(new_antenna);
    emit antenna_data_update( *this, *new_antenna );

    return true;
}

void Radio::antennaDataChanged(Antenna *antenna)
{
    emit antenna_data_update( *this, *antenna );
}

void Radio::delete_antennas(void)
{
    qDeleteAll(m_antennas);
    m_antennas.clear();
    emit radio_data_update(*this);
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
    auto antenna = new Antenna();

    in >> count;

    for(int index=0; index<count; index++){
        in >> *antenna;
        radio.add_antenna(std::move(antenna));
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

Radios& Radios::operator=(const Radios& radios)
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

bool Radios::add_radio(Radio *radio)
{
    auto rad = std::move(radio);
    //Don't add an antenna that already exists
    foreach(auto rad, m_radios){
        if(rad->m_name == rad->m_name){
            delete(rad);
            return false;
        }
    }

    m_radios.append(rad);
//    connect( rad, SIGNAL(antenna_data_update()), this, SLOT(antenna_data_changed()) );
    connect( rad, &Radio::antenna_data_update, this, &Radios::antenna_data_changed );

    emit radio_data(rad, NULL);
    return true;
}

void Radios::antenna_data_changed(Radio &radio, Antenna &antenna)
{
    emit radio_data(&radio, &antenna);
}

QDataStream &operator<<(QDataStream &out, const Radios &radios)
{
    const char* uuid = radios.RADIOS_UUID;
    out << uuid;

    const int radio_count = radios.m_radios.size();
    out << radio_count;

    for(int index=0; index < radio_count; index++){
        out << *radios.m_radios[index];
    }
}

QDataStream &operator>>(QDataStream &in, Radios &radios)
{
    int radio_count;
    unsigned int uuid_len = sizeof(radios.RADIOS_UUID) / sizeof(radios.RADIOS_UUID[0]);
    char uuid[uuid_len] = "";
    char* uuid_ref = &uuid[0];
    in.readBytes(uuid_ref, uuid_len);

    if(uuid != radios.RADIOS_UUID){
        auto failed_str = QString("Load radios faied UUID:%1").arg( radios.RADIOS_UUID);
        qDebug(failed_str.toLatin1());
    }

     in >> radio_count;

     for(int index=0; index<radio_count; index++){
         auto radio = new Radio();
         in >> *radio;
         radios.add_radio(std::move(radio));
     }
}

//QDataStream &operator<<(QDataStream &out, const TestPattern &pattern)
//{
//    out << pattern.TEST_PATTERN_VERSION;

//    const int radio_count = pattern.m_radios.size();    //pattern.antenna_count();
//    out << radio_count;

//    for(int index=0; index < radio_count; index++){
//        out << *pattern.m_radios[index];
//    }

//    return out;
//}

//QDataStream &operator>>(QDataStream &in, TestPattern &pattern)
//{
//    uint radio_count;
//    uint version;
//    Radio radio;

//    in >> version ;
//    if(version != pattern.TEST_PATTERN_VERSION){
//        qDebug("Input did not match version number.  Did not load");
//        return in;
//    }

//    in >> radio_count;

//    for(int index=0; index<radio_count; index++){
//        in >> radio;
//        pattern.add_radio(radio);
//    }
//    return in;
//}
