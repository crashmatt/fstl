#ifndef ANTENNADATA_H
#define ANTENNADATA_H

#include <QObject>
#include <QColor>
#include <QVector3D>
#include <QMap>
#include <QList>
#include <QVector>

class AntennaDataPoint : public QObject
{
    Q_OBJECT
public:
    explicit AntennaDataPoint(QObject *parent = 0, QVector3D rotation = {0.0,0.0,0.0});

public:
    QVector3D   m_rotation;
    QColor      m_center_color;
    float       m_center_visibility;
    double      m_visibility;

signals:

public slots:

};


class AntennaData : public QObject
{
    Q_OBJECT

public:
    explicit AntennaData(QObject *parent, QVector3D ant_pos, int x_steps, int z_steps);

    QVector<AntennaDataPoint*> m_antenna_data;

protected:
    QVector3D   m_ant_pos;
    int         m_x_axis_steps;
    int         m_z_axis_steps;

signals:

protected:
    void clear(void);

public slots:

};

#endif // ANTENNADATA_H
