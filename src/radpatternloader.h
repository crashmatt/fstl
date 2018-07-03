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
    explicit RadPatternLoader(QObject* parent, const QString& m_filename, const ObjectConfig& config);
    ~RadPatternLoader();
    void run();

    const QString filename() {return m_filename;};

protected:
    RadPatternSet* load_rad_pattern();
    RadPatternPoint* point_from_line(QObject* parent, QByteArray &line, int index);

signals:
    void loaded_file(QString m_filename);
    void got_mesh(Mesh* m, const ObjectConfig& config);
    void got_rad_pattern(RadPatternSet* pattern);
    void error_missing_file();
    void incomplete_pattern(const QString& name);

private:
    const QString       m_filename;
    const ObjectConfig  m_config;
};

#endif // RADPATTERNLOADER_H
