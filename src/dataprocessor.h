#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include "mesh.h"

#include "antennadata.h"

class RadPatternData;
class RadPatternSet;

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent, RadPatternData* patterns);

    bool    m_scale_to_visibility;

protected:
    void build_antenna_visibility_object(AntennaData &data, AntennaConfig &config);
    void build_antenna_effective_object(AntennaData &data, AntennaConfig &config);

    RadPatternData* m_patterns;

    float get_rad_intensity(RadPatternSet* pattern, QVector3D rot);

signals:
    void built_mesh(Mesh* m, QString shader, QColor color, int order, QString name);
    void set_obj_pos(QString& obj_name, QVector3D& pos);

public slots:
    void process_data(AntennaData &data, AntennaConfig &config);
};

#endif // DATAPROCESSOR_H
