#ifndef RADPATTERNDATA_H
#define RADPATTERNDATA_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QMap>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QMultiMap>

class Vertex;

class RadPatternPoint
{
public:
    explicit RadPatternPoint(int index, float theta, float phi, float ver, float hor, float total);

    Vertex make_vertex();
    QColor get_color();
    float get_amplitude();

    float theta;
    float phi;
    float ver;
    float hor;
    float total;
    int   index;
};


////////////////////////////////////////////////////////////////////////////////


class RadPatternSet : public QSharedData
{
public:
    RadPatternSet(QString name);

    const QString               set_name;
    QVector<RadPatternPoint*>   rad_data;
    QVector<RadPatternPoint*>   pattern_map;
    QMap<int, int>              phi_map;
    QMap<int, int>              theta_map;

    bool build_maps();
    RadPatternPoint* get_point(int phi, int theta);
    RadPatternPoint* nearest_point(int phi, int theta);
};


////////////////////////////////////////////////////////////////////////////////


class RadPatternData : public QObject
{
    Q_OBJECT
public:
    explicit RadPatternData(QObject *parent = 0);
    QExplicitlySharedDataPointer<RadPatternSet> get_data(const QString &pattern_name);

signals:

public slots:
    void new_pattern_data(RadPatternSet* data);

protected:
    QMap< QString, QExplicitlySharedDataPointer<RadPatternSet> > pattern_data;
};

#endif // RADPATTERNDATA_H
