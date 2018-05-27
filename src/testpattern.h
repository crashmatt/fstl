#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include <QColor>
#include <QVector3D>
#include <QObject>
#include <QList>

class TestPattern : public QObject
{
    Q_OBJECT
public:
    explicit TestPattern(QObject *parent = 0);

protected:
    bool                m_pattern_running;
    QVector3D           m_rotation;
    QList<QVector3D>    m_antenna_positions;
    int                 m_ant_pos_index;

    bool set_antenna_pos_to_index(int index);

signals:
    void set_rotation(QVector3D rotation);
    void set_obj_pos(QString& obj_name, QVector3D& pos);
    void set_view_pos(QVector3D& pos);
    void set_zoom(float zm);
    void redraw();
    void test_completed();

public slots:
    void center_color(QColor color, QVector3D rotation);
    void reset_pattern(void);
    void start_pattern(void);
    void stop_pattern(void);
    void step_antenna_pos(void);
};

#endif // TESTPATTERN_H
