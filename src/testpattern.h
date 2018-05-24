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

signals:
    void set_rotation(QVector3D rotation);

public slots:
    void center_color(QColor color, QVector3D rotation);
};

#endif // TESTPATTERN_H
