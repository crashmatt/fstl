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
        auto mesh = pattern->create_mesh();
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

