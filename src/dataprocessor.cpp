#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include <QMatrix4x4>

void DataProcessor::process_data(AntennaData* data)
{
    float max_vis = 0.01;
    AntennaDataPoint* datapt;
    for(int x_step; x_step < data->m_x_axis_steps; x_step++){
        for(int z_step; z_step < data->m_z_axis_steps; z_step++){
            datapt = data->get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            if(datapt->m_visibility > max_vis){
                max_vis = datapt->m_color_visibility;
            }
        }
    }
    for(int x_step; x_step < data->m_x_axis_steps; x_step++){
        for(int z_step; z_step < data->m_z_axis_steps; z_step++){
            datapt = data->get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            datapt->m_center_visibility = datapt->m_center_color.greenF();
            datapt->m_visibility = datapt->m_color_visibility / max_vis;
        }
    }

    build_mesh(data);
}

void DataProcessor::build_mesh(AntennaData* data)
{
    uint vertex_count_est = data->m_z_axis_steps * data->m_x_axis_steps;

    std::vector<GLfloat> flat_verts;
    flat_verts.reserve(vertex_count_est*3);
    int vect_index = 0;

    AntennaDataPoint* datapt;
    for(int x_step; x_step < data->m_x_axis_steps; x_step++){
        for(int z_step; z_step < data->m_z_axis_steps; z_step++){
            datapt = data->get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            flat_verts[vect_index] = cos(datapt->m_rotation.x())    * cos(datapt->m_rotation.x());
            flat_verts[vect_index+1] = cos(datapt->m_rotation.z())  * cos(datapt->m_rotation.x());
            flat_verts[vect_index+2] = cos(datapt->m_rotation.z())  * sin(datapt->m_rotation.x());
            vect_index += 3;
        }
    }

//    int tri_count = data->m_z_axis_steps * data->m_x_axis_steps;
//    // This vector will store triangles as sets of 3 indices
//    std::vector<GLuint> indices(tri_count*3);
//    uint index = 0;


//    return new Mesh(std::move(flat_verts), std::move(indices));
}
