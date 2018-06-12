#include "antennadata.h"


AntennaConfig::AntennaConfig(QVector3D pos, QVector3D rot, QString type, QString name)
  : m_pos(pos)
  , m_rotation(rot)
  , m_type(type)
  , m_name(name)
{

}

////////////////////////////////////////////////////////////////////////////////

AntennaDataPoint::AntennaDataPoint(QObject *parent, QVector3D rotation) : QObject(parent)
  , m_rotation(rotation)
  , m_center_color(0, 0, 0, 255)
  , m_center_visibility(0.0)
  , m_visibility(0.0)
{

}

////////////////////////////////////////////////////////////////////////////////


AntennaData::AntennaData(QObject *parent, QVector3D ant_pos, int x_steps, int z_steps, int index) : QObject(parent)
    , m_ant_pos(ant_pos)
    , m_x_axis_steps(x_steps)
    , m_z_axis_steps(z_steps)
    , m_index(index)

{
    m_antenna_data.reserve(x_steps * z_steps);
    m_antenna_data.fill(NULL, x_steps * z_steps);
}

void AntennaData::set_antenna_datapoint(AntennaDataPoint* datapt, int x_step, int z_step)
{
    if(x_step > m_x_axis_steps)
        return;

    if(z_step > m_z_axis_steps)
        return;

    int index = data_index(z_step, x_step);
    m_antenna_data[index] = datapt;
}

void AntennaData::set_antenna_datapoint(AntennaDataPoint* datapt, int index)
{
    if(index >= m_antenna_data.size())
        return;
    m_antenna_data[index] = datapt;
}


AntennaDataPoint* AntennaData::get_antenna_datapoint(int x_step, int z_step)
{
    int index = data_index(z_step, x_step);
    if(index >= m_antenna_data.count())
        return NULL;
    return m_antenna_data[index];
}
