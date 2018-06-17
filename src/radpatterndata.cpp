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
{
};

float RadPatternPoint::get_amplitude()
{
    return pow(10, total*0.1);
}

Vertex RadPatternPoint::make_vertex()
{
    float x_angle = degToRad(theta-90);
    float z_angle = degToRad(phi);
    float x_theta = sin(z_angle);
    float y_theta = cos(z_angle);
    float x_phi = cos(x_angle);
    float z_phi = sin(x_angle);
    float radius = get_amplitude();
    float x =  radius * y_theta * x_phi;
    float y = radius * x_theta * x_phi;
    float z = radius * z_phi;

    return Vertex(x, y, z, index);
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

bool RadPatternSet::build_maps()
{
    index_point_map.clear();
    angle_point_map.clear();

    int phi_index = 0;
    int theta_index = 0;
    foreach(RadPatternPoint* pt, rad_data){
        int phi     = (int) pt->phi;
        int theta   = (int) pt->theta;

        if(!angle_point_map.contains(phi)){
            theta_index = 0;
            phi_index++;
//            angle_point_map[phi] = QMap<int, RadPatternPoint*>;
//            index_point_map[phi_index] = QMap<int, RadPatternPoint*>;
        }
        angle_point_map[phi][theta] = pt;
        index_point_map[phi_index][theta_index] = pt;
        theta_index++;
    }

//    auto total = phi_map.count() * theta_map.count();
//    if(rad_data.count() != total){
//        theta_map.clear();
//        phi_map.clear();
//        return false;
//    }
    return true;
}

RadPatternPoint* RadPatternSet::get_point(int phi, int theta)
{
    if(!angle_point_map.keys().contains(phi)) return NULL;
    if(!angle_point_map.contains(theta)) return NULL;
    return angle_point_map[phi][theta];
}

RadPatternPoint* RadPatternSet::get_point_at_index(uint phi_index, uint theta_index)
{
    if(phi_index >= index_point_map.count())
        return NULL;
    if(theta_index >= index_point_map[phi_index].count())
        return NULL;

    return index_point_map[phi_index][theta_index];
}

RadPatternPoint* RadPatternSet::nearest_point(float phi, float theta)
{
    auto phi_min = angle_point_map[0][0]->phi;
    auto theta_min = angle_point_map[0][0]->theta;
    auto phi_max = angle_point_map.last()[0]->phi;
    auto theta_max = angle_point_map[0].last()->phi;
    auto delta_phi = phi_max - phi_min;
    auto delta_theta = theta_max - theta_min;

    auto phi_point = (phi - phi_min) / delta_phi;
    auto theta_point = (theta - theta_min) / delta_theta;

    phi_point /= (float) angle_point_map.count();
    theta_point /= (float) angle_point_map[0].count();

    int phi_index = floor(phi_point + 0.5);
    int theta_index = floor(theta_point + 0.5);

    return index_point_map[phi_index][theta_index];
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
