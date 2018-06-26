#include "antennadata.h"


Antenna::Antenna(QVector3D pos, QQuaternion rot, QString type, QString name)
  : m_pos(pos)
  , m_rotation(rot)
  , m_type(type)
  , m_name(name)
{

}


void Antenna::deleteAntennaData()
{
    qDeleteAll(m_antenna_data);
    m_antenna_data.clear();
}
////////////////////////////////////////////////////////////////////////////////

AntennaDataPoint::AntennaDataPoint(QObject *parent, const QQuaternion &rot, float center, float visibility) : QObject(parent)
  , m_rot(rot)
  , m_center_color(center)
  , m_center_visibility(0.0)
  , m_color_visibility(visibility)
  , m_visibility(0.0)
{

}


////////////////////////////////////////////////////////////////////////////////


