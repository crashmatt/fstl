#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include <QColor>
#include <QVector3D>
#include <QObject>
#include <QList>
#include <QVector>

#include "antennadata.h"

class TestPattern : public QObject
{
    Q_OBJECT
public:
    explicit TestPattern(QObject *parent = 0);

protected:
    bool                m_pattern_running;
    QVector3D           m_rotation;
    QList<AntennaConfig> m_antenna_configs;
    int                 m_ant_pos_index;
    int                 m_pitch_segments;
    int                 m_pitch_index;
    int                 m_yaw_segments;
    int                 m_yaw_index;
    QVector<AntennaData*> m_results;
    bool                m_high_speed;

    bool set_antenna_pos_to_index(int index);
    void reset();

signals:
    void set_rotation(QVector3D rotation, int index);
    void set_obj_pos(QString& obj_name, QVector3D& pos);
    void set_obj_rotation(QString& obj_name, QVector3D& rot);
    void set_view_pos(QVector3D& pos);
    void set_zoom(float zm);
    void redraw();
    void test_completed();
    void antenna_data(AntennaData &data, AntennaConfig &config);
    void delete_object(QString& obj_name);
    void set_object_visible(QString& obj_name, bool visible);

public slots:
    void antenna_visibility(int index, QVector3D rotation, float center_color, float color_visibility);
    void reset_pattern(void);
    void start_pattern(void);
    void stop_pattern(void);
    void step_antenna_pos(void);
    void set_speed(bool high_speed);
};

#endif // TESTPATTERN_H
