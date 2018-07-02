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


class AntennaDataPoint : public QObject
{
    Q_OBJECT
public:
    AntennaDataPoint();
    AntennaDataPoint(QObject *parent, const QQuaternion &rot, float center, float visibility);
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



class Antenna
{
public:
    explicit Antenna(QVector3D pos, QQuaternion rot, QString m_type, QString m_name, QColor color);
    ~Antenna();

    QVector3D   m_pos;
    QQuaternion m_rotation;
    QString     m_type;
    QString     m_name;
    QColor      m_color;

    QVector<AntennaDataPoint>                   m_antenna_data;
    QExplicitlySharedDataPointer<RadPatternSet> m_rad_pattern;

    void deleteAntennaData();
};


QDataStream &operator<<(QDataStream &, const Antenna &);
QDataStream &operator>>(QDataStream &, Antenna &);


////////////////////////////////////////////////////////////////////////////////


#endif // ANTENNADATA_H
