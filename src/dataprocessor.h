#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QQuaternion>
#include "mesh.h"

class Antenna;
class ObjectConfig;

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent);

protected:
    void build_antenna_visibility_object(Antenna *antenna);
    void build_antenna_effective_object(Antenna *antenna);

    int m_sequence;

signals:
    void built_mesh(Mesh* m, const ObjectConfig& config);
    void set_obj_pos(const QString& obj_name, const QVector3D& pos);

public slots:
    void process_data(Antenna *antenna);
};

#endif // DATAPROCESSOR_H
