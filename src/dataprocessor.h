#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include "mesh.h"

#include "antennadata.h"

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent = 0);

    bool    m_scale_to_visibility;

protected:
    void build_antenna_visibility_object(AntennaData &data, AntennaConfig &config);
    void build_antenna_effective_object(AntennaData &data, AntennaConfig &config);

signals:
    void built_mesh(Mesh* m, QString shader, QColor color, int order, QString name);
    void set_obj_pos(QString& obj_name, QVector3D& pos);

public slots:
    void process_data(AntennaData &data, AntennaConfig &config);
};

#endif // DATAPROCESSOR_H
