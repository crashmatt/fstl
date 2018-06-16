#include <future>
#include <QMap>

#include "radpatternloader.h"
#include "vertex.h"
#include "radpatterndata.h"
#include "globject.h"


RadPatternLoader::RadPatternLoader(QObject* parent, const QString& filename, const ObjectConfig& config)
    : QThread(parent)
    , filename(filename)
    , m_config(config)
{

}

RadPatternLoader::~RadPatternLoader()
{

}

void RadPatternLoader::run()
{
    auto pattern = load_rad_pattern();
    if(pattern){
        auto mesh = create_mesh(pattern);
        if (mesh)
        {
            emit got_mesh(mesh, m_config);
            emit loaded_file(filename);
        }
        emit got_rad_pattern(pattern);
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

    return new RadPatternPoint(index, theta, phi, ver, hor, total);
}


RadPatternSet* RadPatternLoader::load_rad_pattern()
{
    auto rad_pattern = new RadPatternSet(m_config.m_name);

    QVector<RadPatternPoint*>& rad_data = rad_pattern->rad_data;
    rad_data.reserve(36*36);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error_missing_file();
        return NULL;
    }

    file.readLine();    //Ignore first two lines as headers
    file.readLine();

    int index  = 0;
    while(!file.atEnd()){
        auto line = file.readLine();
        RadPatternPoint* radpt = point_from_line(this, line, index);
        if(radpt != NULL){
            rad_data.append(radpt);
        }
        index++;
    }

    if(!rad_pattern->build_maps()){
        emit incomplete_pattern(m_config.m_name);
    }

    return rad_pattern;
}


Mesh* RadPatternLoader::create_mesh(RadPatternSet* rad_pattern)
{
    QVector<RadPatternPoint*>& rad_data = rad_pattern->rad_data;

    QVector< QVector<RadPatternPoint*>* > pt_map;
    QList< int > thetas;
    QList< int > phis;
    int index;

    foreach(RadPatternPoint* radpt, rad_data){
        int int_phi = (int) radpt->phi;
        if(!phis.contains(int_phi)){
            phis.append(int_phi);
            QVector<RadPatternPoint*>* newline = new QVector<RadPatternPoint*>();
            pt_map.append( newline );
        }

        pt_map.last()->append(radpt);
    }

    if(rad_data.isEmpty())
        return NULL;

    const int theta_cnt = pt_map[0]->count();
    const int phi_cnt = pt_map.count();

    const int vertcount = rad_data.size();
    std::vector<GLfloat> flat_verts(6*vertcount);
    index = 0;
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

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    return mesh;
}
