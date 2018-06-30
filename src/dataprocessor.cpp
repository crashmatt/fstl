#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include "radpatterndata.h"
#include "globject.h"
#include <QMatrix4x4>

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

DataProcessor::DataProcessor(QObject *parent) : QObject(parent)
  , m_sequence(20)
{

}

void DataProcessor::process_data(Antenna *antenna)
{
    Q_ASSERT(antenna != NULL);
    auto pattern = antenna->m_rad_pattern.data();
    Q_ASSERT(pattern != NULL);

    float max_vis = 0.01;
    foreach(AntennaDataPoint* datapt , antenna->m_antenna_data){
        Q_ASSERT(datapt != NULL);
        if(datapt->m_color_visibility > max_vis){
            max_vis = datapt->m_color_visibility;
        }
    }
    foreach(AntennaDataPoint* datapt, antenna->m_antenna_data){
        datapt->m_center_visibility = datapt->m_center_color.greenF();
        datapt->m_visibility = datapt->m_color_visibility / max_vis;
    }

    build_antenna_visibility_object(antenna);
    build_antenna_effective_object(antenna);
}


void DataProcessor::build_antenna_visibility_object(Antenna *antenna)
{
    RadPatternSet* pattern = antenna->m_rad_pattern.data();
    Q_ASSERT(pattern != NULL);

    std::vector<GLfloat> flat_verts(2 * 3 * antenna->m_antenna_data.size());

    QColor viscolor = antenna->m_color;

    AntennaDataPoint* datapt;
    int index = 0;
    int vect_index = 0;
    foreach(datapt , antenna->m_antenna_data){
        Q_ASSERT(datapt != NULL);
        vect_index = index * 6;
        auto rot = datapt->m_rot;
        auto vect = rot.rotatedVector(QVector3D(0.0, 0.0, -1.0)) * datapt->m_visibility;
        flat_verts[vect_index] =  vect.x();
        flat_verts[vect_index+1] = vect.y();
        flat_verts[vect_index+2] = vect.z();
        float color_scale = datapt->m_visibility * datapt->m_visibility;
        flat_verts[vect_index+3] = color_scale * viscolor.redF();
        flat_verts[vect_index+4] = color_scale * viscolor.greenF();
        flat_verts[vect_index+5] = color_scale * viscolor.blueF();
        index++;
    }

    std::vector<GLuint> indices;

    pattern->make_indices(indices);

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    QString name = QString("ant_vis_%1").arg(antenna->m_name);

    auto mesh_config = ObjectConfig(name, "visi", antenna->m_color, m_sequence++);
    mesh_config.m_offset = antenna->m_pos;
    emit built_mesh(mesh, mesh_config);
}


void DataProcessor::build_antenna_effective_object(Antenna *antenna)
{
    RadPatternSet* pattern = antenna->m_rad_pattern.data();
    Q_ASSERT(pattern != NULL);

    std::vector<GLfloat> flat_verts(12 * antenna->m_antenna_data.size());

    QColor viscolor = antenna->m_color;

    int index = 0;
    int vect_index = 0;
    foreach(auto datapt , antenna->m_antenna_data){
        Q_ASSERT(datapt != NULL);
        vect_index = index * 12;
        auto rad_strength = pattern->rad_data[index]->get_amplitude();
        auto rot = datapt->m_rot;
        auto vect = rot.rotatedVector(QVector3D(0.0, 0.0, -1.0)) * rad_strength * datapt->m_visibility;
        flat_verts[vect_index] =  vect.x();
        flat_verts[vect_index+1] = vect.y();
        flat_verts[vect_index+2] = vect.z();

        float color_scale = datapt->m_visibility * datapt->m_visibility;
        flat_verts[vect_index+3] = color_scale * viscolor.redF();
        flat_verts[vect_index+4] = color_scale * viscolor.greenF();
        flat_verts[vect_index+5] = color_scale * viscolor.blueF();

        flat_verts[vect_index+6] = 1.0;
        flat_verts[vect_index+7] = 0.0;
        flat_verts[vect_index+8] = 0.0;

        flat_verts[vect_index+9] = 0.0;
        flat_verts[vect_index+10] = 1.0;
        flat_verts[vect_index+11] = 0.0;
        index++;
    }

    std::vector<GLuint> indices;
    pattern->make_indices(indices);

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 12);
    QString name = QString("ant_eff_%1").arg(antenna->m_name);

    auto mesh_config = ObjectConfig(name, "radpattern", antenna->m_color, m_sequence++);
    mesh_config.m_offset = antenna->m_pos;
    emit built_mesh(mesh, mesh_config);
}

