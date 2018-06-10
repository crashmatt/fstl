#ifndef RADPATTERNLOADER_H
#define RADPATTERNLOADER_H

#include <QObject>
#include <QThread>
#include <QColor>
#include <QString>
#include <QByteArray>

#include "mesh.h"
class Vertex;

class RadPatternPoint : public QObject
{
    Q_OBJECT
public:
    explicit RadPatternPoint(QObject* parent, int index, float theta, float phi, float ver, float hor, float total);
//    bool parse_line(QByteArray line);
    static RadPatternPoint* from_line(QObject* parent, QByteArray &line, int index);

    Vertex make_vertex();
    QColor get_color();

    float theta;
    float phi;
    float ver;
    float hor;
    float total;
    int   index;
};



class RadPatternLoader : public QThread
{
    Q_OBJECT
public:
    explicit RadPatternLoader(QObject* parent, const QString& filename, const QString& obj_name, const QString& frag_shader, const QColor &color, int order);
    void run();

protected:
    Mesh* load_rad_pattern();

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
