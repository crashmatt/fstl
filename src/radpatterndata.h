#ifndef RADPATTERNDATA_H
#define RADPATTERNDATA_H

#include <QObject>
#include <QColor>

class Vertex;

class RadPatternPoint : public QObject
{
    Q_OBJECT
public:
    explicit RadPatternPoint(QObject* parent, int index, float theta, float phi, float ver, float hor, float total);
//    bool parse_line(QByteArray line);

    Vertex make_vertex();
    QColor get_color();

    float theta;
    float phi;
    float ver;
    float hor;
    float total;
    int   index;
};


class RadPatternData : public QObject
{
    Q_OBJECT
public:
    explicit RadPatternData(QObject *parent = 0);

signals:

public slots:
};

#endif // RADPATTERNDATA_H
