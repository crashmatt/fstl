#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QQuaternion>
#include "mesh.h"

#include "antennadata.h"

class RadPatternData;
class RadPatternSet;
class ObjectConfig;

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent, RadPatternData* patterns);

protected:
    void build_antenna_visibility_object(AntennaData &data, const AntennaConfig &config);
    void build_antenna_effective_object(AntennaData &data, const AntennaConfig &config);

    RadPatternData* m_patterns;

    float get_rad_intensity(RadPatternSet* pattern, QQuaternion rot);

signals:
    void built_mesh(Mesh* m, const ObjectConfig& config);
    void set_obj_pos(const QString& obj_name, const QVector3D& pos);

public slots:
    void process_data(AntennaData &data, AntennaConfig &config);
};

#endif // DATAPROCESSOR_H
