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
    QMap<int, QMap<int, RadPatternPoint*> > angle_point_map;
    QMap<int, QMap<int, RadPatternPoint*> > index_point_map;

    bool build_maps();
    RadPatternPoint* get_point(int phi, int theta);
    RadPatternPoint* get_point_at_index(uint phi_index, uint theta_index);
    RadPatternPoint* nearest_point(float phi, float theta);
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
