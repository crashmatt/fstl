#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include <QColor>
#include <QVector3D>
#include <QObject>
#include <QList>
#include <QVector>
#include <QQuaternion>

#include "antennadata.h"
#include "time.h"

class Radios;
class Radio;
class RadPatternData;
class RadioSimResults;

class TestPattern : public QObject
{
    Q_OBJECT
public:    
    explicit TestPattern(QObject *parent, Radios *radios);
    ~TestPattern();

protected:
    Radios*             m_radios;
    RadioSimResults*    m_sim_results;

    bool                m_pattern_running;

    int                 m_ant_pos_index;
    int                 m_test_index;
    bool                m_high_speed;

    bool                m_rotations_running;
    int                 m_step_index;

    bool set_antenna_pos_to_index(int index);
    void reset();
//    void rotation_step(double delta_time_ms);

signals:
    void set_rotation(QQuaternion rotation, int index);
    void set_obj_pos(QString& obj_name, QVector3D& pos);
    void set_obj_rotation(QString& obj_name, QQuaternion& rot);
    void set_view_pos(QVector3D& pos);
    void set_zoom(float zm);
    void redraw();
    void test_completed();
    void delete_object(QString& obj_name);
    void set_object_visible(QString& obj_name, bool visible);
    void antenna_debug_text(QStringList& data);

public slots:
    void antenna_visibility(int index, QQuaternion rotation, float center_color, float color_visibility);
    void reset_pattern(void);
    void start_pattern(void);
    void stop_pattern(void);
    void step_antenna_pos(void);
    void set_speed(bool high_speed);
    void start_rotations(QString filename);
};



#endif // TESTPATTERN_H
