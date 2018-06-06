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
    float       m_color_visibility;
    float       m_visibility;

signals:

public slots:

};


class AntennaData : public QObject
{
    Q_OBJECT

public:
    explicit AntennaData(QObject *parent, QVector3D ant_pos, int x_steps, int z_steps, int index);

    const int   m_x_axis_steps;
    const int   m_z_axis_steps;
    void set_antenna_datapoint(AntennaDataPoint* datapt, int x_step, int z_step);
    void set_antenna_datapoint(AntennaDataPoint* datapt, int index);
    AntennaDataPoint* get_antenna_datapoint(int x_step, int z_step);
    int data_index(int z_step, int x_step) {return z_step + (x_step * m_z_axis_steps);}
    int index() {return m_index;};
    QVector3D position() {return m_ant_pos;};

    const int   m_index;

protected:
    QVector<AntennaDataPoint*> m_antenna_data;
    QVector3D   m_ant_pos;

signals:

protected:
    void clear(void);

public slots:

};

#endif // ANTENNADATA_H
