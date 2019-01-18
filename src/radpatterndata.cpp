#include "radpatterndata.h"
#include "mesh.h"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

RadPatternPoint::RadPatternPoint(int i, float t, float p, float v, float h, float tot)
  : theta(t)
  , phi(p)
  , ver(v)
  , hor(h)
  , total(tot)
  , index(i)
  , rot()
{
    rot =   QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, phi) * QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, theta);
};

float RadPatternPoint::get_amplitude()
{
    return pow(10, total*0.1);
}

float RadPatternPoint::get_vertical()
{
    return pow(10, ver*0.1);
}

float RadPatternPoint::get_horizontal()
{
    return pow(10, hor*0.1);
}

Vertex RadPatternPoint::make_vertex()
{
    auto vect = rot.rotatedVector(QVector3D(0.0, 0.0, 1.0));
    vect *= get_amplitude();
    return Vertex(vect.x(), vect.y(), vect.z(), index);
}

QColor RadPatternPoint::get_color()
{
    QColor color(0,0,0,125);
    color.setRedF( pow(10, ver*0.2 )*0.25);
    color.setGreenF(pow(10, hor*0.2 )*0.25);
    color.setBlueF(1.0-pow(10, total*0.2 )*0.25);
    return color;
}

////////////////////////////////////////////////////////////////////////////////

RadPatternSet::RadPatternSet(QString name)
    : set_name(name)
{

}

RadPatternSet::~RadPatternSet()
{
    //Clear these so referenced contents are not automatically deleted
    angle_index_map.clear();
    id_index_map.clear();
    phis.clear();
    thetas.clear();
}

RadPatternSet::index_id RadPatternSet::get_id(int phi, int theta)
{
    RadPatternSet::index_id id;
    id._.phi = phi;
    id._.theta = theta;
    return id;
}

bool RadPatternSet::build_maps()
{
    angle_index_map.clear();
    id_index_map.clear();
    phis.clear();
    thetas.clear();

    int phi_index = 0;
    int theta_index = 0;
    int pt_index = 0;
    foreach(RadPatternPoint* pt, rad_data){
        const int phi     = (int) pt->phi;
        const int theta   = (int) pt->theta;
        auto id = get_id(phi, theta);
        angle_index_map[id.id] = pt_index;

        if(!phis.contains(phi)){
            phis.append(phi);
            phi_index = phis.size() - 1;
            theta_index = 0;
        } else {
            theta_index++;
        }
        if(!thetas.contains(theta)){
            thetas.append(theta);
        }

        id = get_id(phi_index, theta_index);
        id_index_map[id.id] = pt_index;
        pt_index++;
    }

    auto total = phis.count() * thetas.count();
    if(rad_data.count() != total){
        id_index_map.clear();
        angle_index_map.clear();
        phis.clear();
        thetas.clear();
        return false;
    }
    return true;
}

//RadPatternPoint* RadPatternSet::get_point(int phi, int theta)
//{
//    const auto id = get_id(phi, theta);
//    return rad_data[angle_index_map.value(id.id, NULL)];
//}

//RadPatternPoint* RadPatternSet::get_point_at_index(uint phi_index, uint theta_index)
//{
//    const auto id = get_id(phi_index, theta_index);
//    return rad_data[id_index_map.value(id.id, NULL)];
//}

RadPatternPoint* RadPatternSet::nearest_point(QQuaternion rot)
{
    if(rad_data.isEmpty()) return NULL;
    double nearest_dist = 1000;
    RadPatternPoint* nearest = rad_data[0];
    const auto vect = rot.inverted().rotatedVector(QVector3D(0.0, 1.0, 0.0));

    foreach(RadPatternPoint* pt, rad_data){
        auto ptvect = pt->rot.inverted().rotatedVector(QVector3D(1.0, 0.0, 0.0));
//        auto rotdiff = rot * pt->rot.inverted();
//        auto vectdiff = rotdiff.rotatedVector(QVector3D(0.0, 0.0, 1.0));
        auto diffs = (ptvect - vect);
        diffs *= diffs;
        auto diff = diffs.length();
        if(diff < nearest_dist){
            nearest_dist = diff;
            nearest = pt;
        }
    }

    return nearest;
}


Mesh* RadPatternSet::create_mesh()
{
    if(rad_data.isEmpty())
        return NULL;

    const int vertcount = rad_data.size();
    std::vector<GLfloat> flat_verts(6*vertcount);
    int index = 0;
    for(int i=0; i<vertcount; i++){
        Vertex vertex = rad_data[i]->make_vertex();
        QColor color = rad_data[i]->get_color();
        flat_verts[index] = vertex.x;
        flat_verts[index+1] = vertex.y;
        flat_verts[index+2] = vertex.z;
        flat_verts[index+3] = color.redF();
        flat_verts[index+4] = color.greenF();
        flat_verts[index+5] = color.blueF();
        index += 6;
    }

    std::vector<GLuint> indices;
    make_indices(indices);

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    return mesh;
}


void RadPatternSet::make_indices(std::vector<GLuint>& indices)
{
    const int theta_cnt = thetas.size();
    const int phi_cnt = phis.size();
    int tri_count = (theta_cnt-1) * (phi_cnt-1) * 2;

    indices.resize(tri_count*3);

    int index = 0;
    uint index00, index01, index11, index10;
    for(int p=0; p<(phi_cnt-1); p++){
        for(int t=0; t<(theta_cnt-1); t++){
            index00 = id_index_map.value(get_id(p, t).id, 0);
            index01 = id_index_map.value(get_id(p, t+1).id, 0);
            index11 = id_index_map.value(get_id(p+1, t+1).id, 0);
            index10 = id_index_map.value(get_id(p+1, t).id, 0);
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index +=6;
        }
    }
}

void RadPatternSet::write(QJsonObject &json) const
{
}

////////////////////////////////////////////////////////////////////////////////


RadPatternData::RadPatternData(QObject *parent) : QObject(parent)
{
}

QExplicitlySharedDataPointer<RadPatternSet> RadPatternData::get_data(const QString &pattern_name)
{
    return pattern_data.value( pattern_name, QExplicitlySharedDataPointer<RadPatternSet>() );
}

void RadPatternData::add_pattern_data(RadPatternSet* data)
{
    Q_ASSERT(data != NULL);
    if(pattern_data.contains(data->set_name)){
        pattern_data.remove(data->set_name);
    }
    pattern_data[data->set_name] = data;
}

void RadPatternData::write(QJsonObject &json) const
{
//    QJsonArray patterns;

//    foreach (const auto pattern, pattern_data.values()) {
//        QJsonObject patternObject;
//        pattern.write(patternObject);
//        patterns.append(patternObject);
//    }
//    json["levels"] = levelArray;
}

QDataStream &operator<<(QDataStream &, const RadPatternData &)
{

}

QDataStream &operator>>(QDataStream &, RadPatternData &){

}
