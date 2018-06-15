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
#include "globject.h"

class Vertex;

class RadPatternLoader : public QThread
{
    Q_OBJECT
public:
    explicit RadPatternLoader(QObject* parent, const QString& filename, const ObjectConfig& config);
    ~RadPatternLoader();
    void run();

protected:
    RadPatternSet* load_rad_pattern();
    Mesh* create_mesh(RadPatternSet* pattern);
    RadPatternPoint* point_from_line(QObject* parent, QByteArray &line, int index);

signals:
    void loaded_file(QString filename);
    void got_mesh(Mesh* m, const ObjectConfig& config);
    void got_rad_pattern(RadPatternSet* pattern);
    void error_missing_file();

private:
    const QString       filename;
    const ObjectConfig  m_config;
};

#endif // RADPATTERNLOADER_H
