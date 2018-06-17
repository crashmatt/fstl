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

    union index_id {
        struct _t{
            int16_t phi;
            int16_t theta;
        } _;
        int32_t     id;
    };

public:
    RadPatternSet(QString name);
    ~RadPatternSet();

    const QString               set_name;
    QVector<RadPatternPoint*>   rad_data;

    bool build_maps();
    RadPatternPoint* get_point(int phi, int theta);
    RadPatternPoint* get_point_at_index(uint phi_index, uint theta_index);
    RadPatternPoint* nearest_point(float phi, float theta);

protected:
    index_id get_id(int phi, int theta);
    QMap<uint32_t, RadPatternPoint*>    angle_point_map;
    QMap<uint32_t, RadPatternPoint*>    index_point_map;
    QVector<int> phis;
    QVector<int> thetas;
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
