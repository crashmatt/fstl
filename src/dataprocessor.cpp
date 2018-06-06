#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include <QMatrix4x4>

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

DataProcessor::DataProcessor(QObject *parent) : QObject(parent)
  , m_scale_to_visibility(true)

{

}

void DataProcessor::process_data(AntennaData &data)
{
    float max_vis = 0.01;
    AntennaDataPoint* datapt;
    for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
        for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
            datapt = data.get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            if(datapt->m_color_visibility > max_vis){
                max_vis = datapt->m_color_visibility;
            }
        }
    }
    for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
        for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
            datapt = data.get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            datapt->m_center_visibility = datapt->m_center_color.greenF();
            datapt->m_visibility = datapt->m_color_visibility / max_vis;
        }
    }

    build_mesh(data);
}

void DataProcessor::build_mesh(AntennaData &data)
{
    std::vector<GLfloat> flat_verts(2 * 3 * data.m_z_axis_steps * data.m_x_axis_steps);

    AntennaDataPoint* datapt;
    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
        for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
            datapt = data.get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            float x_angle = degToRad(-datapt->m_rotation.x());
            float z_angle = degToRad(datapt->m_rotation.z()  - 90.0);
            int vect_index = 2 * 3 * data.data_index(z_step, x_step);
            float x_theta = sin(z_angle);
            float y_theta = cos(z_angle);
            float x_phi = cos(x_angle);
            float z_phi = sin(x_angle);
            float radius = 1.0;
            if(m_scale_to_visibility){
                radius = datapt->m_visibility;
            }
            flat_verts[vect_index] =  radius * y_theta * x_phi;
            flat_verts[vect_index+1] = radius * x_theta * x_phi;
            flat_verts[vect_index+2] = radius * z_phi;
            flat_verts[vect_index+3] = datapt->m_visibility;
            flat_verts[vect_index+4] = 0.0;
            flat_verts[vect_index+5] = 1-datapt->m_visibility;
        }
    }

    int tri_count = (data.m_z_axis_steps) * (data.m_x_axis_steps) * 2;
    // This vector will store triangles as sets of 3 indices
    std::vector<GLuint> indices(tri_count*3);

    uint index = 0;
    uint index00, index01, index11, index10;
    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
        for(int x_step=0; x_step < data.m_x_axis_steps-1; x_step++){
            index00 = data.data_index(z_step, x_step);
            index01 = data.data_index(z_step+1, x_step);
            index11 = data.data_index(z_step+1, x_step+1);
            index10 = data.data_index(z_step, x_step+1);
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index += 6;
        }
    }

//    std::vector<GLfloat> flat_verts(6 * 4);
//    std::vector<GLuint> indices(2*3);

//    uint index = 0;
//    flat_verts[index+0] = -1.0;
//    flat_verts[index+1] = -1.0;
//    flat_verts[index+2] = 0.0;
//    flat_verts[index+3] = 1.0;
//    flat_verts[index+4] = 0.0;
//    flat_verts[index+5] = 0.0;
//    index += 6;
//    flat_verts[index+0] = -1.0;
//    flat_verts[index+1] = 1.0;
//    flat_verts[index+2] = 0.0;
//    flat_verts[index+3] = 0.0;
//    flat_verts[index+4] = 1.0;
//    flat_verts[index+5] = 0.0;
//    index += 6;
//    flat_verts[index+0] = 1.0;
//    flat_verts[index+1] = -1.0;
//    flat_verts[index+2] = 0.0;
//    flat_verts[index+3] = 0.0;
//    flat_verts[index+4] = 0.0;
//    flat_verts[index+5] = 1.0;
//    index += 6;
//    flat_verts[index+0] = 1.0;
//    flat_verts[index+1] = 1.0;
//    flat_verts[index+2] = 0.0;
//    flat_verts[index+3] = 0.5;
//    flat_verts[index+4] = 0.5;
//    flat_verts[index+5] = 0.5;

//    indices[0] = 0;
//    indices[1] = 1;
//    indices[2] = 2;
//    indices[3] = 1;
//    indices[4] = 2;
//    indices[5] = 3;

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    QString name = "ant_vis" + data.index();
    QVector3D position = data.position();

    emit built_mesh(mesh, "visi", QColor(255,0,0,128), 10+data.index(), name);
    emit set_obj_pos(name, position);
}
