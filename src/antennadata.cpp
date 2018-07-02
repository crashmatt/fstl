#include "antennadata.h"
#include "radpatterndata.h"


Antenna::Antenna()
    : m_pos()
    , m_rotation()
    , m_type("none")
    , m_name("no-name")
    , m_color("black")
    , m_rad_pattern()
{

}

Antenna::Antenna(QVector3D pos, QQuaternion rot, QString type, QString name, QColor color)
  : m_pos(pos)
  , m_rotation(rot)
  , m_type(type)
  , m_name(name)
  , m_color(color)
  , m_rad_pattern()
{

}

Antenna::Antenna(const Antenna& antenna)
    : m_pos(antenna.m_pos)
    , m_rotation(antenna.m_rotation)
    , m_type(antenna.m_type)
    , m_name(antenna.m_name)
    , m_color(antenna.m_color)
    , m_rad_pattern(antenna.m_rad_pattern)
    , m_antenna_data(antenna.m_antenna_data)
{

}

Antenna::~Antenna()
{
    deleteAntennaData();
}


void Antenna::deleteAntennaData()
{
//    qDeleteAll(m_antenna_data);
    m_antenna_data.clear();
}

QDataStream &operator<<(QDataStream &out, const Antenna &antenna)
{
    out << antenna.m_pos << antenna.m_rotation
        << antenna.m_type << antenna.m_name
        << antenna.m_color << antenna.m_antenna_data;

    return out;
}

QDataStream &operator>>(QDataStream &in, Antenna &antenna)
{
    QVector3D   pos;
    QQuaternion rotation;
    QString     type;
    QString     name;
    QColor      color;
    QVector<AntennaDataPoint> antenna_data;

    in >> pos >> rotation
        >> type >> name
        >> color >> antenna_data;

    antenna.m_pos = pos;
    antenna.m_rotation = rotation;
    antenna.m_type = type;
    antenna.m_name = name;
    antenna.m_color = color;
    antenna.m_antenna_data = antenna_data;

    return in;
}


////////////////////////////////////////////////////////////////////////////////

AntennaDataPoint::AntennaDataPoint() : QObject(NULL)
  , m_rot()
  , m_center_color(0,0,0,0)
  , m_center_visibility(0.0)
  , m_color_visibility(0.0)
  , m_visibility(0.0)
{
}

AntennaDataPoint::AntennaDataPoint(QObject *parent, const QQuaternion &rot, float center, float visibility) : QObject(parent)
  , m_rot(rot)
  , m_center_color(center)
  , m_center_visibility(0.0)
  , m_color_visibility(visibility)
  , m_visibility(0.0)
{
}

AntennaDataPoint::AntennaDataPoint(const AntennaDataPoint& antenna) : QObject(antenna.parent())
  , m_rot(antenna.m_rot)
  , m_center_color(antenna.m_center_color)
  , m_center_visibility(antenna.m_center_visibility)
  , m_color_visibility(antenna.m_color_visibility)
  , m_visibility(antenna.m_visibility)
{
}

AntennaDataPoint& AntennaDataPoint::operator=(const AntennaDataPoint& antenna)
{
    m_rot = antenna.m_rot;
    m_center_color = antenna.m_center_color;
    m_center_visibility = antenna.m_center_visibility;
    m_color_visibility = antenna.m_color_visibility;
    m_visibility = antenna.m_visibility;
}

QDataStream &operator<<(QDataStream &out, const AntennaDataPoint &datapt)
{
    out << datapt.m_rot << datapt.m_center_color
        << datapt.m_center_visibility << datapt.m_color_visibility
        << datapt.m_visibility;
    return out;
}

QDataStream &operator>>(QDataStream &in, AntennaDataPoint &datapt)
{
    QQuaternion   rot;
    QColor        center_color;
    float         color_visibility;
    float         center_visibility;
    float         visibility;

    in >> datapt.m_rot >> datapt.m_center_color
        >> datapt.m_center_visibility >> datapt.m_color_visibility
        >> datapt.m_visibility;
    return in;
}


////////////////////////////////////////////////////////////////////////////////


