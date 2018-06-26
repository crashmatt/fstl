#include "radpatterndata.h"
#include "vertex.h"

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
    rot =   QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, theta) *
            QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, phi);
};

float RadPatternPoint::get_amplitude()
{
    return pow(10, total*0.1);
}

Vertex RadPatternPoint::make_vertex()
{
//    float x_angle = degToRad(theta-90);
//    float z_angle = degToRad(phi);
//    float x_theta = sin(z_angle);
//    float y_theta = cos(z_angle);
//    float x_phi = cos(x_angle);
//    float z_phi = sin(x_angle);
//    float radius = get_amplitude();
//    float x =  radius * y_theta * x_phi;
//    float y = radius * x_theta * x_phi;
//    float z = radius * z_phi;

//    return Vertex(x, y, z, index);

    auto vect = rot.inverted().rotatedVector(QVector3D(1.0, 0.0, 0.0));
    vect *= get_amplitude();
    return Vertex(vect.z(), vect.y(), vect.x(), index);
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

RadPatternPoint* RadPatternSet::get_point(int phi, int theta)
{
    const auto id = get_id(phi, theta);
    return rad_data[angle_index_map.value(id.id, NULL)];
}

RadPatternPoint* RadPatternSet::get_point_at_index(uint phi_index, uint theta_index)
{
    const auto id = get_id(phi_index, theta_index);
    return rad_data[id_index_map.value(id.id, NULL)];
}

RadPatternPoint* RadPatternSet::nearest_point(QQuaternion rot, int phi, int theta)
{
    Q_UNUSED(phi)
    Q_UNUSED(theta)

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
//    auto phi_min = phis[0];
//    auto theta_min = thetas[0];
//    auto phi_max = phis.last();
//    auto theta_max = thetas.last();
//    auto delta_phi = phi_max - phi_min;
//    auto delta_theta = theta_max - theta_min;

//    auto phi_point = (phi - phi_min) / delta_phi;
//    auto theta_point = (theta - theta_min) / delta_theta;

//    phi_point *= (float) phis.count();
//    theta_point *= (float) thetas.count();

//    int phi_index = floor(phi_point + 0.5);
//    int theta_index = floor(theta_point + 0.5);

//    const auto id = get_id(phi_index, theta_index);
//    return index_point_map[id.id];
}


////////////////////////////////////////////////////////////////////////////////


RadPatternData::RadPatternData(QObject *parent) : QObject(parent)
{
}

QExplicitlySharedDataPointer<RadPatternSet> RadPatternData::get_data(const QString &pattern_name)
{
    return pattern_data.value( pattern_name, QExplicitlySharedDataPointer<RadPatternSet>() );
}

void RadPatternData::new_pattern_data(RadPatternSet* data)
{
    Q_ASSERT(data != NULL);
    if(pattern_data.contains(data->set_name)){
        pattern_data.remove(data->set_name);
    }
    pattern_data[data->set_name] = data;
}
