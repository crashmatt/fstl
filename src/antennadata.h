#ifndef ANTENNADATA_H
#define ANTENNADATA_H

#include <QObject>
#include <QColor>
#include <QVector3D>
#include <QMap>
#include <QList>

class AntennaDataPoint : public QObject
{
    Q_OBJECT
public:
    explicit AntennaDataPoint(QObject *parent = 0, QVector3D rotation = {0.0,0.0,0.0}, float center_visibility=0.0, float visibility=0.0);

public:
    QVector3D   m_rotation;
    float       m_center_visibility;
    double      m_visibility;

signals:

public slots:

};


class AntennaData : public QObject
{
    Q_OBJECT

public:
    explicit AntennaData(QObject *parent = 0, QColor antenna_color = {0,255,0,255});

protected:
    QColor  m_antenna_color;

signals:

protected:
    void clear(void);

public slots:

};

#endif // ANTENNADATA_H
