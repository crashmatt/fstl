#ifndef RADPATTERNDATA_H
#define RADPATTERNDATA_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QQuaternion>
#include <QMap>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QMultiMap>

class Vertex;
class Mesh;

class RadPatternPoint
{
public:
    explicit RadPatternPoint(int index, float theta, float phi, float ver, float hor, float total);

    Vertex make_vertex();
    QColor get_color();
    float get_amplitude();

    const float theta;
    const float phi;
    const float ver;
    const float hor;
    const float total;
    const int   index;
    QQuaternion rot;
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
    RadPatternPoint* nearest_point(QQuaternion rot, int phi=-1, int theta=-1);
    Mesh* create_mesh();

protected:
    index_id get_id(int phi, int theta);
    QMap<uint32_t, int>    angle_index_map;
    QMap<uint32_t, int>    id_index_map;
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
    void add_pattern_data(RadPatternSet* data);

protected:
    QMap< QString, QExplicitlySharedDataPointer<RadPatternSet> > pattern_data;
};

#endif // RADPATTERNDATA_H
