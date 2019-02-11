#include "antennadata.h"
#include "radpatterndata.h"
#include <QDebug>
#include <QtMath>


Antenna::Antenna() : QObject(NULL)
    , m_pos()
    , m_rotation()
    , m_type("none")
    , m_name("no-name")
    , m_color("black")
    , m_rad_pattern()
{
    m_rad_pattern = RadPatternData::get_instance()->get_data(m_type);
}

Antenna::Antenna(QObject *parent, QVector3D pos, QQuaternion rot, QString type, QString name, QColor color) : QObject(NULL)
  , m_pos(pos)
  , m_rotation(rot)
  , m_type(type)
  , m_name(name)
  , m_color(color)
  , m_rad_pattern()
{
    m_rad_pattern = RadPatternData::get_instance()->get_data(m_type);
}

Antenna::Antenna(const Antenna& antenna) : QObject(antenna.parent())
    , m_pos(antenna.m_pos)
    , m_rotation(antenna.m_rotation)
    , m_type(antenna.m_type)
    , m_name(antenna.m_name)
    , m_color(antenna.m_color)
    , m_rad_pattern(antenna.m_rad_pattern)
    , m_antenna_data(antenna.m_antenna_data)
    , m_rotation_config(antenna.m_rotation_config)
{
    m_rad_pattern = RadPatternData::get_instance()->get_data(m_type);
}


Antenna::Antenna(QObject *parent, const QJsonObject& json): QObject(parent)
    , m_pos(0.0,0.0,0.0)
    , m_rotation(QQuaternion())
    , m_type("none")
    , m_name("unknown")
    , m_color(QColor("black"))
    , m_antenna_data()
    , m_rotation_config()
{
    m_name = json["name"].toString();
    m_type = json["type"].toString();
    m_rad_pattern = RadPatternData::get_instance()->get_data(m_type);

    QJsonObject colour = json["colour"].toObject();
    m_color = QColor(colour["R"].toDouble(), colour["G"].toDouble(), colour["B"].toDouble());

    QJsonObject pos = json["pos"].toObject();
    m_pos = QVector3D(pos["X"].toDouble(), pos["Y"].toDouble(), pos["Z"].toDouble());

    QJsonObject rotation = json["rotation"].toObject();
    auto quat_vect = QVector4D(rotation["Xval"].toDouble(), rotation["Yval"].toDouble(), rotation["Zval"].toDouble(), rotation["Wval"].toDouble());
    m_rotation = QQuaternion(quat_vect);

    QJsonArray rotations = json["conf_rotations"].toArray();
    foreach(const QJsonValue & rotVal, rotations){
        QJsonObject rotObj = rotVal.toObject();
        auto rot = QVector4D(rotObj["X"].toDouble(), rotObj["Y"].toDouble(), rotObj["Z"].toDouble(), rotObj["Angle"].toDouble());
        m_rotation_config.append(rot);
    }
}


Antenna::~Antenna()
{
    deleteAntennaData();
}

void Antenna::addRotation(QVector3D axis, float angle)
{
    m_rotation_config.append(QVector4D(axis, angle));
    m_rotation = QQuaternion::fromAxisAndAngle(axis, angle) * m_rotation;
}

void Antenna::deleteAntennaData()
{
//    qDeleteAll(m_antenna_data);
    m_antenna_data.clear();
}

void Antenna::write_config(QJsonObject &json) const
{
    QJsonValue name(m_name);
    json["name"] = name;

    QJsonValue type(m_type);
    json["type"] = type;

    QJsonObject colour;
    colour["R"] = QJsonValue(m_color.red());
    colour["G"] = QJsonValue(m_color.green());
    colour["B"] = QJsonValue(m_color.blue());
    json["colour"] = colour;

    QJsonObject pos;
    QJsonValue Xpos(m_pos.x());
    QJsonValue Ypos(m_pos.y());
    QJsonValue Zpos(m_pos.z());
    pos["X"] = Xpos;
    pos["Y"] = Ypos;
    pos["Z"] = Zpos;
    json["pos"] = pos;

//    QVector3D   axis;
//    float       angle;
//    m_rotation.getAxisAndAngle(&axis, &angle);
//    QJsonObject rot;
//    QJsonValue Xrot(qRadiansToDegrees(axis.x()));
//    QJsonValue Yrot(qRadiansToDegrees(axis.y()));
//    QJsonValue Zrot(qRadiansToDegrees(axis.z()));
//    rot["X"] = Xrot;
//    rot["Y"] = Yrot;
//    rot["Z"] = Zrot;
//    rot["angle"] = angle;
    QJsonArray rotations;
    foreach(auto rotation, m_rotation_config){
        QJsonObject conf_rot;
        conf_rot["X"] = QJsonValue(rotation.x());
        conf_rot["Y"] = QJsonValue(rotation.y());
        conf_rot["Z"] = QJsonValue(rotation.z());
        conf_rot["Angle"] = QJsonValue(rotation.w());
        rotations.append(conf_rot);
    }
    json["conf_rotations"] = rotations;

    QJsonObject rot;
    QVector4D quat = m_rotation.toVector4D();
    QJsonValue Xval(quat.x());
    QJsonValue Yval(quat.y());
    QJsonValue ZVal(quat.z());
    QJsonValue WVal(quat.w());
    rot["Xval"] = Xval;
    rot["Yval"] = Yval;
    rot["Zval"] = ZVal;
    rot["Wval"] = WVal;
    json["rotation"] = rot;
}


QVector3D Antenna::radiationVector(QQuaternion rotation)
{
    //View rotation relative to phi=theta=0 null rotation at the antenna
    auto ant_quot_rot = rotationToAntennaFrame(rotation);

    //Nearest antenna datapoint to the view rotation onto the antenna
    auto nearest = nearestFromAntennaRotation(ant_quot_rot);
//    auto nearest = m_rad_pattern.data()->nearest_point(ant_quot_rot);
//    auto ant_nearest_str = QString("%1 Rad Phi:%2 theta:%3 amp:%4").
//            arg(m_name,10).
//            arg(nearest->phi,3,'f',1).
//            arg(nearest->theta,3,'f',1).
//            arg(nearest->get_amplitude(),4,'f',2);
//    qDebug( ant_nearest_str.toLatin1() );

    auto rad_data_index = m_rad_pattern.data()->get_index(nearest->phi, nearest->theta);
    auto& rad_data = m_antenna_data[rad_data_index];
    auto effective = rad_data.m_visibility * nearest->get_amplitude();
//   auto effecive3D = rad_data.m_visibility * nearest->get_rad_vector();
    //TODO - Effective should be 3D vector

//    auto ant_rad_str = QString("%1 Rad index:%2 vis:%3 eff:%4")
//            .arg(m_name,10)
//            .arg(rad_data_index)
//            .arg(rad_data.m_visibility,4,'f',2)
//            .arg(effective,4,'f',2);
//    qDebug( ant_rad_str.toLatin1() );

    //Antenna phi=theta=0 vector rotated with view rotation
    auto ant_rad_rot = rotationToAntennaVector(rotation);
    auto ant_vect = antennaRotationVector(ant_rad_rot);

//    auto ant_vect_str = QString("%1 Vect X:%2 Y:%3 Z:%4").
//            arg(m_name,10).
//            arg(ant_vect.x(),5,'f',2,' ').
//            arg(ant_vect.y(),5,'f',2,' ').
//            arg(ant_vect.z(),5,'f',2,' ');
//    qDebug( ant_vect_str.toLatin1() );

    //Effective radiation vector
    auto ant_rad_vect = ant_vect;
    ant_rad_vect.normalize();
    ant_rad_vect *= effective;

    return ant_rad_vect;
}


QDataStream &operator<<(QDataStream &out, const Antenna &antenna)
{
    out << antenna.m_pos << antenna.m_rotation
        << antenna.m_type << antenna.m_name
        << antenna.m_color << antenna.m_antenna_data;

    out << antenna.m_rotation_config;

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
    QVector<QVector4D> rotation_config;

    in >> pos >> rotation
        >> type >> name
        >> color >> antenna_data;

    in >> rotation_config;

    antenna.m_pos = pos;
    antenna.m_rotation = rotation;
    antenna.m_type = type;
    antenna.m_name = name;
    antenna.m_color = color;
    antenna.m_antenna_data = antenna_data;

    antenna.m_rad_pattern = RadPatternData::get_instance()->get_data(antenna.m_type);

    return in;
}


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

AntennaDataPoint::AntennaDataPoint()
  : m_rot()
  , m_center_color(0,0,0,0)
  , m_center_visibility(0.0)
  , m_color_visibility(0.0)
  , m_visibility(0.0)
{
}

AntennaDataPoint::AntennaDataPoint(const QQuaternion &rot, float center, float visibility)
  : m_rot(rot)
  , m_center_color(center)
  , m_center_visibility(0.0)
  , m_color_visibility(visibility)
  , m_visibility(0.0)
{
}

AntennaDataPoint::AntennaDataPoint(const AntennaDataPoint& antenna)
  : m_rot(antenna.m_rot)
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


