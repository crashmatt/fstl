#ifndef RADPATTERNLOADER_H
#define RADPATTERNLOADER_H

#include <QObject>
#include <QThread>
#include <QColor>
#include <QString>
#include <QByteArray>
#include <QExplicitlySharedDataPointer>

#include "mesh.h"
#include "radpatterndata.h"
class Vertex;

class RadPatternLoader : public QThread
{
    Q_OBJECT
public:
    explicit RadPatternLoader(QObject* parent, const QString& filename, const QString& obj_name, const QString& frag_shader, const QColor &color, int order);
    ~RadPatternLoader();
    void run();

protected:
    Mesh* load_rad_pattern();
    RadPatternPoint* point_from_line(QObject* parent, QByteArray &line, int index);

signals:
    void loaded_file(QString filename);
    void got_mesh(Mesh* m, QString shader, QColor color, int order, QString name);
    void got_rad_pattern(RadPatternSet& pattern);
    void error_missing_file();

private:
    const QString filename;
    const QString frag_shader;
    const QColor  base_color;
    const int     show_order;
    const QString name;
    QExplicitlySharedDataPointer<RadPatternSet> rad_pattern_ptr;
};

#endif // RADPATTERNLOADER_H
