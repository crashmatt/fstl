#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include <QColor>
#include <QVector3D>
#include <QObject>

class TestPattern : public QObject
{
    Q_OBJECT
public:
    explicit TestPattern(QObject *parent = 0);

protected:
    bool pattern_running;
    QVector3D m_rotation;
    QVector3D m_view_offset;

signals:
    void set_rotation(QVector3D rotation);
    void set_obj_pos(QString& obj_name, QVector3D& pos);
    void set_view_pos(QVector3D& pos);
    void test_completed();

public slots:
    void center_color(QColor color, QVector3D rotation);
    void reset_pattern(void);
    void start_pattern(void);
    void stop_pattern(void);
};

#endif // TESTPATTERN_H
