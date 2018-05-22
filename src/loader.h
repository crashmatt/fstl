#ifndef LOADER_H
#define LOADER_H

#include <QThread>
#include <QColor>

#include "mesh.h"

class Loader : public QThread
{
    Q_OBJECT
public:
    explicit Loader(QObject* parent, const QString& filename, const QString& obj_name, const QString& frag_shader, const QColor &color, int order);
    void run();

protected:
    Mesh* load_stl();

    /*  Reads an ASCII stl, starting from the start of the file*/
    Mesh* read_stl_ascii(QFile& file);
    /*  Reads a binary stl, assuming we're at the end of the header */
    Mesh* read_stl_binary(QFile& file);

signals:
    void loaded_file(QString filename);
    void got_mesh(Mesh* m, QString shader, QColor color, int order, QString name);

    void error_bad_stl();
    void error_empty_mesh();
    void warning_confusing_stl();
    void error_missing_file();

private:
    const QString filename;
    const QString frag_shader;
    const QColor  base_color;
    const int     show_order;
    const QString name;
    /*  Used to warn on binary STLs that begin with the word 'solid'" */
    bool confusing_stl;

};

#endif // LOADER_H
