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
    phi_map.clear();
    theta_map.clear();
    foreach(RadPatternPoint* pt, rad_data){
        int phi     = (int) pt->phi;
        int theta   = (int) pt->theta;
        if(!phi_map.contains(phi)){
            phi_map[phi] = phi_map.size();
        }
        if(!theta_map.contains(theta)){
            theta_map[theta] = theta_map.size();
        }
    }
    auto total = phi_map.count() * theta_map.count();
    if(rad_data.count() != total){
        theta_map.clear();
        phi_map.clear();
        return false;
    }
    return true;
}

RadPatternPoint* RadPatternSet::get_point(int phi, int theta)
{
    auto phi_index = phi_map.value(phi, -1);
    auto theta_index = theta_map.value(theta, -1);

    if(phi_index == -1)
        return NULL;
    if(theta_index == -1)
        return NULL;

    auto theta_count = theta_map.size();

    auto index = theta_index + (phi_index * theta_count);
    if(index >= rad_data.count())
        return NULL;
    return rad_data[index];
}


RadPatternPoint* RadPatternSet::nearest_point(int phi, int theta)
{
    if(theta_map.empty() || phi_map.empty())
        return NULL;

    int nearest_phi = -1;
    int nearest_theta = -1;
    auto phi_index = 0;
    auto theta_index = 0;

    foreach(int phi_key, phi_map.keys()){
        int delta_phi = abs(phi_key - phi);
        if(delta_phi < nearest_phi){
            phi_index = phi_map[phi_key];
        }
    }

    foreach(int theta_key, theta_map.keys()){
        int delta_theta = abs(theta_key - theta);
        if(delta_theta < nearest_theta){
            theta_index = theta_map[theta_key];
        }
    }

    auto theta_count = theta_map.size();

    auto index = theta_index + (phi_index * theta_count);
    if(index >= rad_data.count())
        return NULL;
    return rad_data[index];
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
