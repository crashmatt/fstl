#include "radiosimresults.h"


RadioSimResults::RadioSimResults(Radios* radios)

{
}



bool RadioSimResults::loadPathFile(QString filename)
{
    QFile loadFile(filename);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file to load.");
        return false;
    }

    QByteArray bytes = loadFile.readAll();
    MsgPackStream packstream(&bytes, QIODevice::ReadOnly);

    double x,y,z,w = 0;

    m_rotation_sets.clear();
    m_position_sets.clear();

    quint32 set_count;
    quint32 length;
    packstream >> set_count;
    m_rotation_sets.reserve(set_count);
    for(auto set=0; set<set_count; set++){
        packstream >> length;
        QList<QQuaternion> rotations;
        rotations.reserve(length);
        for(auto i=0; i<length; i++){
            packstream >> w;
            packstream >> x;
            packstream >> y;
            packstream >> z;
            rotations.append(QQuaternion(w,x,y,z));
        }
        m_rotation_sets.append(rotations);
    }

    packstream >> set_count;
    packstream >> length;
    m_position_sets.reserve(length);
    for(auto set=0; set<set_count; set++){
        packstream >> length;
        QList<QVector3D> positions;
        positions.reserve(length);
        for(auto i=0; i<length; i++){
            packstream >> x;
            packstream >> y;
            packstream >> z;
            positions.append(QVector3D(x,y,z));
        }
        m_position_sets.append(positions);
    }

    //Unpack rotations between object positions
    quint32 src_radio, sink_radio;
    do{
        packstream >> src_radio;
        packstream >> sink_radio;
        packstream >> length;
        QList<QQuaternion> pos_rotations;
        for(auto i=0; i<length; i++){
            packstream >> w;
            packstream >> x;
            packstream >> y;
            packstream >> z;
            pos_rotations.append(QQuaternion(w,x,y,z));
        }
        if(length > 0){
            m_pos_rotations[x][y] = pos_rotations;
        }
    } while(length > 0);

    QStringList names;
    packstream >> names;

    loadFile.close();

    return true;
}

