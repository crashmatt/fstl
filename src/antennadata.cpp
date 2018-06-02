#include "antennadata.h"

AntennaDataPoint::AntennaDataPoint(QObject *parent, QVector3D rotation) : QObject(parent)
  , m_rotation(rotation)
  , m_center_color(0, 0, 0, 255)
  , m_center_visibility(0.0)
  , m_visibility(0.0)
{

}

AntennaData::AntennaData(QObject *parent, QVector3D ant_pos, int x_steps, int z_steps) : QObject(parent)
    , m_ant_pos(ant_pos)
    , m_x_axis_steps(x_steps)
    , m_z_axis_steps(z_steps)

{
    m_antenna_data.reserve(x_steps * z_steps);
}

