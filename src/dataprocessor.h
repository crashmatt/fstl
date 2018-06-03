#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>

#include "antennadata.h"

class DataProcessor : public QObject
{
    Q_OBJECT
//public:
//    explicit DataProcessor(QObject *parent = 0);

protected:
    static void build_mesh(AntennaData* data);

signals:

public slots:
    static void process_data(AntennaData* data);
};

#endif // DATAPROCESSOR_H
