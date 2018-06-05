#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include <QMatrix4x4>

DataProcessor::DataProcessor(QObject *parent) : QObject(parent)
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
            if(datapt->m_visibility > max_vis){
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
    std::vector<GLfloat> flat_verts(3 * data.m_z_axis_steps * data.m_x_axis_steps);

    AntennaDataPoint* datapt;
    for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
        for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
            int vect_index = 3 * data.data_index(z_step, x_step);
            datapt = data.get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            flat_verts[vect_index] = sin(datapt->m_rotation.z())    * cos(datapt->m_rotation.x());
            flat_verts[vect_index+1] = cos(datapt->m_rotation.z())  * cos(datapt->m_rotation.x());
            flat_verts[vect_index+2] = cos(datapt->m_rotation.z())  * sin(datapt->m_rotation.x());
        }
    }

    int tri_count = (data.m_z_axis_steps-1) * (data.m_x_axis_steps-1) * 2;
    // This vector will store triangles as sets of 3 indices
    std::vector<GLuint> indices(tri_count*3);
    uint index = 0;

    uint index00, index01, index11, index10;
    for(int x_step=0; x_step < data.m_x_axis_steps-1; x_step++){
        for(int z_step=0; z_step < data.m_z_axis_steps-1; z_step++){
            index00 = data.data_index(x_step, z_step);
            index01 = data.data_index(x_step, z_step+1);
            index11 = data.data_index(x_step+1, z_step+1);
            index10 = data.data_index(x_step+1, z_step);
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index += 6;
        }
    }

//    std::vector<GLfloat> flat_verts;
//    flat_verts.reserve(4*3);
//    flat_verts.push_back(0.0);
//    flat_verts.push_back(0.0);
//    flat_verts.push_back(0.0);

//    flat_verts.push_back(1.0);
//    flat_verts.push_back(0.0);
//    flat_verts.push_back(0.0);

//    flat_verts.push_back(1.0);
//    flat_verts.push_back(1.0);
//    flat_verts.push_back(0.0);

//    flat_verts.push_back(0.0);
//    flat_verts.push_back(1.0);
//    flat_verts.push_back(0.0);

//    std::vector<GLuint> indices(2*3);
//    indices[0] = 0;
//    indices[1] = 1;
//    indices[2] = 3;
//    indices[3] = 1;
//    indices[4] = 2;
//    indices[5] = 3;

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices));
    QString name = "ant_vis" + data.index();
    emit built_mesh(mesh, "solid", QColor(255,0,0,128), 10+data.index(), name);
}
