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

#include "vertex.h"

class Vertex;
class Mesh;

class RadPatternPoint
{
public:
    explicit RadPatternPoint(int index, float theta, float phi, float ver, float hor, float total);

    Vertex make_vertex();
    QColor get_color();
    float get_amplitude() {return pow(10, total*0.1);}
    float get_vertical() {return pow(10, ver*0.1);}
    float get_horizontal() {return pow(10, hor*0.1);}

    QVector3D get_rad_vector() {return QVector3D(get_horizontal(), get_vertical(), 0.0); }

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
    int get_index(int phi, int theta);
    RadPatternPoint* nearest_point(QQuaternion rot);
    Mesh* create_mesh();
    void make_indices(std::vector<GLuint>& indices);
    void write(QJsonObject &json) const;

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
    static RadPatternData* get_instance() {return RadPatternData::s_instance;};

    void write(QJsonObject &json) const;

signals:

public slots:
    void add_pattern_data(RadPatternSet* data);

protected:
    QMap< QString, QExplicitlySharedDataPointer<RadPatternSet> > m_pattern_data;

    static RadPatternData* s_instance;
};

#endif // RADPATTERNDATA_H
