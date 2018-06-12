#include "radpatterndata.h"
#include "vertex.h"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

RadPatternPoint::RadPatternPoint(QObject* parent, int i, float t, float p, float v, float h, float tot) : QObject(parent)
  , theta(t)
  , phi(p)
  , ver(v)
  , hor(h)
  , total(tot)
  , index(i)
{
    ;
};

Vertex RadPatternPoint::make_vertex()
{
    float amplitude = pow(10, total*0.1);
    float x_angle = degToRad(theta-90);
    float z_angle = degToRad(phi);
    float x_theta = sin(z_angle);
    float y_theta = cos(z_angle);
    float x_phi = cos(x_angle);
    float z_phi = sin(x_angle);
    float radius = amplitude;
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


RadPatternData::RadPatternData(QObject *parent) : QObject(parent)
{

}
