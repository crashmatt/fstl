#include "radpatternloader.h"

#include <QFile>

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


Mesh* RadPatternLoader::load_rad_pattern()
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error_missing_file();
        return NULL;
    }

    while(!file.atEnd()){
        const auto line = file.readLine().trimmed();
        QList<QByteArray> list = line.split(',');
        if(list.count() > 10){
            float theta, phi, vert, hor, total;
            list[0].number(theta);
            list[1].number(phi);
            list[2].number(vert);
            list[3].number(hor);
            list[4].number(total);
        }
    }
    return NULL;
}
