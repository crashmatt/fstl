#include <future>
#include <QMap>

#include "radpatternloader.h"
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



RadPatternLoader::RadPatternLoader(QObject* parent, const QString& filename, const QString& obj_name, const QString& frag_shader, const QColor& color, int order)
    : QThread(parent)
    , filename(filename)
    , frag_shader(frag_shader)
    , base_color(color)
    , show_order(order)
    , name(obj_name)
{

}

void RadPatternLoader::run()
{
    Mesh* mesh = load_rad_pattern();
    if (mesh)
    {
        emit got_mesh(mesh, frag_shader, base_color, show_order, name);
        emit loaded_file(filename);
    }
}


RadPatternPoint* RadPatternLoader::point_from_line(QObject* parent, QByteArray &line, int index)
{
    const auto parts = line.trimmed().split(',');
    if(parts.count() < 8){
        return NULL;
    }

    bool ok;
    float theta = parts[0].toFloat(&ok);
    if(!ok) return NULL;
    float phi = parts[1].toFloat(&ok);
    if(!ok) return NULL;
    float ver = parts[2].toFloat(&ok);
    if(!ok) return NULL;
    float hor = parts[3].toFloat(&ok);
    if(!ok) return NULL;
    float total = parts[4].toFloat(&ok);
    if(!ok) return NULL;

    return new RadPatternPoint(parent, index, theta, phi, ver, hor, total);
}


Mesh* RadPatternLoader::load_rad_pattern()
{
    QVector<RadPatternPoint*> radpts;
    radpts.reserve(36*36);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error_missing_file();
        return NULL;
    }

    QVector< QVector<RadPatternPoint*>* > pt_map;
    QList< int > thetas;
    QList< int > phis;

    file.readLine();    //Ignore first two lines as headers
    file.readLine();

    int index  = 0;
    while(!file.atEnd()){
        auto line = file.readLine();
        RadPatternPoint* radpt = point_from_line(this, line, index);
        if(radpt != NULL){
            radpts.append(radpt);
            int int_phi = (int) radpt->phi;
            if(!phis.contains(int_phi)){
                phis.append(int_phi);
                QVector<RadPatternPoint*>* newline = new QVector<RadPatternPoint*>();
                pt_map.append( newline );
            }

            pt_map.last()->append(radpt);
            index++;

        }

    }

    if(radpts.isEmpty())
        return NULL;

    const int theta_cnt = pt_map[0]->count();
    const int phi_cnt = pt_map.count();

    const int vertcount = radpts.size();
    std::vector<GLfloat> flat_verts(6*vertcount);
    index = 0;
    for(int i=0; i<vertcount; i++){
        Vertex vertex = radpts[i]->make_vertex();
        QColor color = radpts[i]->get_color();
        flat_verts[index] = vertex.x;
        flat_verts[index+1] = vertex.y;
        flat_verts[index+2] = vertex.z;
        flat_verts[index+3] = color.redF();
        flat_verts[index+4] = color.greenF();
        flat_verts[index+5] = color.blueF();
        index += 6;
    }

    int tri_count = (theta_cnt-1) * (phi_cnt-1) * 2;
    std::vector<GLuint> indices(tri_count*3);

    index = 0;
    uint index00, index01, index11, index10;
    for(int t=0; t<(theta_cnt-1); t++){
        for(int p=0; p<(phi_cnt-1); p++){
            index00 = pt_map[p]->at(t)->index;
            index01 = pt_map[p]->at(t+1)->index;
            index11 = pt_map[p+1]->at(t+1)->index;
            index10 = pt_map[p+1]->at(t)->index;
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index +=6;
        }
    }

    qDeleteAll(pt_map);
    pt_map.clear();

    qDeleteAll(radpts);
    radpts.clear();

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    return mesh;
}
