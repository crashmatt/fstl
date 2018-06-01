#include "antennadata.h"

AntennaDataPoint::AntennaDataPoint(QObject *parent, QVector3D rotation, float center_visibility, float visibility) : QObject(parent)
  , m_rotation(rotation)
  , m_center_visibility(center_visibility)
  , m_visibility(visibility)
{

}

AntennaData::AntennaData(QObject *parent, QColor antenna_color) : QObject(parent)
  , m_antenna_color(antenna_color)
{

}

