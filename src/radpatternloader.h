#ifndef RADPATTERNLOADER_H
#define RADPATTERNLOADER_H

#include <QThread>
#include <QColor>
#include <QString>

#include "mesh.h"
class Vertex;

class RadPatternLoader : public QThread
{
    Q_OBJECT
public:
    explicit RadPatternLoader(QObject* parent, const QString& filename, const QString& obj_name, const QString& frag_shader, const QColor &color, int order);
    void run();

protected:
    Mesh* load_rad_pattern();

    Mesh* mesh_from_verts(uint32_t tri_count, QVector<Vertex>& verts);
    void parallel_sort(Vertex* begin, Vertex* end, int threads);

signals:
    void loaded_file(QString filename);
    void got_mesh(Mesh* m, QString shader, QColor color, int order, QString name);
    void error_missing_file();

private:
    const QString filename;
    const QString frag_shader;
    const QColor  base_color;
    const int     show_order;
    const QString name;
};

#endif // RADPATTERNLOADER_H
