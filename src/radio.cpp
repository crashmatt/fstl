#include "radio.h"

Q_DECLARE_METATYPE(Radio)

Radio::Radio() : QObject(NULL)
  , m_name("unknown")
  , m_pos(0.0, 0.0, 0.0)
{
    qRegisterMetaType<Radio>();
    qRegisterMetaTypeStreamOperators<Radio>("Radio");
}

Radio::Radio(QObject *parent, QString name, QVector3D pos, bool fixed) : QObject(parent)
  , m_name(name)
  , m_pos(pos)
  , m_fixed(fixed)
{
    qRegisterMetaType<Radio>();
    qRegisterMetaTypeStreamOperators<Radio>("Radio");
}


Radio::Radio(QObject *parent, QJsonObject &json) : QObject(parent)
  , m_name("")
  , m_pos(0.0,0.0,0.0)
  , m_fixed(false)
{
    qRegisterMetaType<Radio>();
    qRegisterMetaTypeStreamOperators<Radio>("Radio");

    m_name = json["name"].toString();
    QJsonObject pos = json["pos"].toObject();
    auto X = pos["X"].toDouble();
    auto Y = pos["Y"].toDouble();
    auto Z = pos["Z"].toDouble();
    m_pos = QVector3D(X,Y,Z);

    QJsonArray antennas = json["antennas"].toArray();
    foreach(const QJsonValue & antVal, antennas){
        auto antObj = antVal.toObject();
        auto antenna = new Antenna(this, antObj);
        add_antenna(std::move(antenna));
    }

    QJsonArray objects = json["objects"].toArray();
    foreach(const QJsonValue & objVal, objects){
        auto objStr = objVal.toString();
        add_object(objStr);
    }
}


Radio::Radio(const Radio& radio) : QObject(radio.parent())
    ,m_name(radio.m_name)
    ,m_pos(radio.m_pos)
    ,m_antennas(radio.m_antennas)
    ,m_fixed(radio.m_fixed)
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
    m_fixed = radio.m_fixed;
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

    m_antennas.append(new_antenna);
    new_antenna->setParent(this);
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

void Radio::deleteAntennaData()
{
    foreach(Antenna* antenna, m_antennas){
        antenna->deleteAntennaData();
    }
}



void Radio::write_config(QJsonObject &json) const
{
    QJsonValue name(m_name);
    json["name"] = name;

    QJsonObject pos;
    QJsonValue Xpos(m_pos.x());
    QJsonValue Ypos(m_pos.y());
    QJsonValue Zpos(m_pos.z());
    pos["X"] = Xpos;
    pos["Y"] = Ypos;
    pos["Z"] = Zpos;
    json["pos"] = pos;

    QJsonArray antennas;
    foreach (const auto antenna, m_antennas) {
        QJsonObject antennaObject;
        antenna->write_config(antennaObject);
        antennas.append(antennaObject);
    }
    json["antennas"] = antennas;

    QJsonArray objects;
    foreach (const auto objname, m_objects) {
        QJsonObject object;
        objects.append(objname);
    }
    json["objects"] = objects;

}

void Radio::pack(MsgPackStream &s)
{
    s << m_name;
    s << m_antennas.size();

    foreach(auto antenna, m_antennas){
        antenna->pack(s);
    }
}

void Radio::pack_all(MsgPackStream &s)
{
    s << (uint) Radio::RADIO_VERSION;
    s << m_name;
    s << m_antennas.size();

    foreach(auto antenna, m_antennas){
        antenna->pack_all(s);
    }
}


QDataStream &operator<<(QDataStream &out, const Radio &radio)
{
    out << Radio::RADIO_VERSION;

    const int count = radio.m_antennas.size();    //pattern.antenna_count();
    out << count;

    for(int index=0; index < count; index++){
        out << *radio.m_antennas[index];
    }

//    const int objects_count = radio.m_objects.size();
    out << radio.m_objects;

//    for(int index=0; index < objects_count; index++){
//        out << *radio.m_antennas[index];
//    }

    return out;
}

QDataStream &operator>>(QDataStream &in, Radio &radio)
{
    uint version = 0;
    in >> version;

    if(version != Radio::RADIO_VERSION) {
        in.setStatus(QDataStream::ReadCorruptData);
        return in;
    }

    int count;
    in >> count;

    for(int index=0; index<count; index++){
        auto antenna = new Antenna();
        in >> *antenna;
        radio.add_antenna(std::move(antenna));
    }

    in >> radio.m_objects;

    return in;
}

//MsgPackStream &operator<<(MsgPackStream &s, const Radio &radio)
//{
//    s << radio.m_antennas.size();

//    foreach(auto antenna, radio.m_antennas){
//        s << antenna;
//    }
//    return s;
//}



////////////////////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(Radios)

Radios::Radios() : QObject(NULL)
{
    qRegisterMetaType<Radios>();
    qRegisterMetaTypeStreamOperators<Radios>("Radios");
}

Radios::Radios(QObject *parent)  : QObject(parent)
{
    qRegisterMetaType<Radios>();
    qRegisterMetaTypeStreamOperators<Radios>("Radios");
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
    rad->setParent(this);
    //Don't add an antenna that already exists
    foreach(auto r, m_radios){
        if(rad->m_name == r->m_name){
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
    out << Radios::RADIOS_VERSION;

    const int radio_count = radios.m_radios.size();
    out << radio_count;

    for(int index=0; index < radio_count; index++){
        out << *radios.m_radios[index];
    }
    return out;
}

void Radios::load_config(const QJsonObject &json)
{
    delete_radios();
    if(!json.keys().contains("radios")) return;
    if(!json["radios"].isArray()) return;
    QJsonArray radios = json["radios"].toArray();
    foreach(const QJsonValue & radioVal, radios){
        if(radioVal.isObject()){
            QJsonObject radioObj = radioVal.toObject();
            auto new_rad = new Radio(this, radioObj);
            add_radio( std::move( new_rad ) );
        }
    }
}

void Radios::write_config(QJsonObject &json) const
{
    QJsonArray radios;
    foreach (const auto radio, m_radios) {
        QJsonObject radioObject;
        radio->write_config(radioObject);
        radios.append(radioObject);
    }
    json["radios"] = radios;
}

void Radios::pack(MsgPackStream &s)
{
    s << m_radios.size();

    foreach(auto radio, m_radios){
        radio->pack(s);
    }
}

void Radios::pack_all(MsgPackStream &s)
{
    s << m_radios.size();

    foreach(auto radio, m_radios){
        radio->pack_all(s);
    }
}


QDataStream &operator>>(QDataStream &in, Radios &radios)
{
    uint version = 0;
    in >> version;

    if(version != Radios::RADIOS_VERSION) {
        in.setStatus(QDataStream::ReadCorruptData);
        return in;
    }

    int radio_count;
    in >> radio_count;

    for(int index=0; index<radio_count; index++){
        auto radio = new Radio();
        in >> *radio;
        radios.add_radio(std::move(radio));
    }
}

//MsgPackStream &operator<<(MsgPackStream &s, const Radios & radios)
//{
//    s << radios.m_radios.size();

//    foreach(auto radio, radios.m_radios){
//        s << radio;
//    }
//    return s;
//}

