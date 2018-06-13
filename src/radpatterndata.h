#ifndef RADPATTERNDATA_H
#define RADPATTERNDATA_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QMap>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>

class Vertex;

class RadPatternPoint
{
public:
    explicit RadPatternPoint(int index, float theta, float phi, float ver, float hor, float total);

    Vertex make_vertex();
    QColor get_color();

    float theta;
    float phi;
    float ver;
    float hor;
    float total;
    int   index;
};


class RadPatternSet : public QSharedData
{
public:
    QVector<RadPatternPoint*>   rad_data;
    QString                     set_name;
};

class RadPatternData : public QObject
{
    Q_OBJECT
public:
    explicit RadPatternData(QObject *parent = 0);
    QExplicitlySharedDataPointer<RadPatternSet> get_data(QString &pattern_name);

signals:

public slots:
    void new_pattern_data(RadPatternSet &data);

protected:
    QMap< QString, QExplicitlySharedDataPointer<RadPatternSet> > pattern_data;
};

#endif // RADPATTERNDATA_H
