#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include <QColor>
#include <QVector3D>
#include <QObject>
#include <QList>
#include <QVector>
#include <QQuaternion>

#include "antennadata.h"
#include "radiosimulation.h"
#include "time.h"

class Radio;

class TestPattern : public QObject
{
    Q_OBJECT
public:    
    typedef struct {
        double      start_time;
        double      end_time;
        double      rate;
        QVector3D   direction;
    } rotation_segment;

    explicit TestPattern(QObject *parent = 0);
    ~TestPattern();

    bool add_radio(Radio &radio);
    void delete_data(void);

    const uint   TEST_PATTERN_VERSION = 1;

protected:
    bool                m_pattern_running;
    QList<Radio*>       m_radios;

    int                 m_ant_pos_index;
    int                 m_test_index;
    bool                m_high_speed;

    rotation_segment    m_rotation_segment;
    double              m_last_rotation_time;
    double              m_rotation_timeout;
    bool                m_rotations_running;
    QQuaternion         m_rotation;

    bool set_antenna_pos_to_index(int index);
    void reset();
    void rotation_step(double delta_time_ms);

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
    void start_rotations(const double timeout);

private:
    friend QDataStream & operator<<(QDataStream &os, const TestPattern& p);
    friend QDataStream & operator>>(QDataStream &os, TestPattern& p);

    RadioSimulation* m_radio_simulation;
};


QDataStream &operator<<(QDataStream &, const TestPattern &);
QDataStream &operator>>(QDataStream &, TestPattern &);


#endif // TESTPATTERN_H
