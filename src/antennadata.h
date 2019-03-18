#ifndef ANTENNADATA_H
#define ANTENNADATA_H

#include <QObject>
#include <QColor>
#include <QVector3D>
#include <QQuaternion>
#include <QMap>
#include <QList>
#include <QVector>

#include "radpatterndata.h"

////////////////////////////////////////////////////////////////////////////////


class AntennaDataPoint
{
public:
    AntennaDataPoint();
    explicit AntennaDataPoint(const QQuaternion &rot, float center, float visibility);
    AntennaDataPoint(const AntennaDataPoint&);
    AntennaDataPoint& operator=(const AntennaDataPoint&);

public:
    QQuaternion         m_rot;
    QColor              m_center_color;
    float               m_color_visibility;
    float               m_center_visibility;
    float               m_visibility;

signals:

public slots:

};

QDataStream &operator<<(QDataStream &, const AntennaDataPoint &);
QDataStream &operator>>(QDataStream &, AntennaDataPoint &);

////////////////////////////////////////////////////////////////////////////////



class Antenna : public QObject
{
public:
    Antenna();
    explicit Antenna(QObject *parent, QVector3D pos, QQuaternion rot, QString m_type, QString m_name, QColor color);
    explicit Antenna(QObject *parent, const QJsonObject& json);
    ~Antenna();
    Antenna(const Antenna&);
//    Antenna& operator=(const Antenna&);

    QVector3D   m_pos;
    QQuaternion m_rotation;
    QString     m_type;
    QString     m_name;
    QColor      m_color;

    QVector<AntennaDataPoint>                   m_antenna_data;
    QVector<QVector4D>                          m_rotation_config;
    QExplicitlySharedDataPointer<RadPatternSet> m_rad_pattern;

    void deleteAntennaData();
    QVector3D radiationVector(QQuaternion rotation);

    void write_config(QJsonObject &json) const;

    void addRotation(QVector3D axis, float angle);

    //View rotation relative to phi=theta=0 null rotation at the antenna
    QQuaternion rotationToAntennaFrame(QQuaternion rotation) {return m_rotation.inverted() * rotation ;}

    //Antenna phi=theta=0 rotation wrt. to viewpoint
    QQuaternion rotationToAntennaVector(QQuaternion rotation) {return rotation.inverted() * m_rotation;}

    //Antenna phi=theta=0 vector rotated with view rotation
    QVector3D antennaRotationVector(QQuaternion antenna_rotation) {return antenna_rotation.rotatedVector(QVector3D(0.0, 0.0, -1.0));}

    //Antenna radiation vector rotated with view rotation
    QVector3D antennaRadiationVector(QQuaternion antenna_rotation) {return antenna_rotation.rotatedVector(QVector3D(0.0, 0.0, -1.0));}

    RadPatternPoint* nearestFromAntennaRotation(QQuaternion rotation) {return m_rad_pattern.data()->nearest_point(rotation);}
};


QDataStream &operator<<(QDataStream &, const Antenna &);
QDataStream &operator>>(QDataStream &, Antenna &);


////////////////////////////////////////////////////////////////////////////////


#endif // ANTENNADATA_H
