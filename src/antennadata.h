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
    explicit AntennaDataPoint(QObject *parent, const QQuaternion &rot, float center, float visibility);

public:
    const QQuaternion   m_rot;
    const QColor        m_center_color;
    const float         m_color_visibility;
    float               m_center_visibility;
    float               m_visibility;

signals:

public slots:

};


////////////////////////////////////////////////////////////////////////////////



class Antenna
{
public:
    explicit Antenna(QVector3D pos, QQuaternion rot, QString m_type, QString m_name);
    ~Antenna();

    QVector3D   m_pos;
    QQuaternion m_rotation;
    QString     m_type;
    QString     m_name;

    QVector<AntennaDataPoint*>                  m_antenna_data;
    QExplicitlySharedDataPointer<RadPatternSet> m_rad_pattern;

    void deleteAntennaData();
};


////////////////////////////////////////////////////////////////////////////////


#endif // ANTENNADATA_H
