#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include <QMatrix4x4>

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

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
//    std::vector<GLfloat> flat_verts(3 * data.m_z_axis_steps * data.m_x_axis_steps);

//    AntennaDataPoint* datapt;
//    for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
//        for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
//            int vect_index = 3 * data.data_index(z_step, x_step);
//            datapt = data.get_antenna_datapoint(x_step, z_step);
//            Q_ASSERT(datapt != NULL);
//            float x_theta = cos(datapt->m_rotation.z());
//            float y_theta = sin(datapt->m_rotation.z());
//            float x_phi = cos(datapt->m_rotation.x());
//            float y_phi = sin(datapt->m_rotation.x());
//            flat_verts[vect_index] = x_theta * x_phi;
//            flat_verts[vect_index+1] = y_theta * x_phi;
//            flat_verts[vect_index+2] = y_phi;
//        }
//    }

//    int tri_count = (data.m_z_axis_steps-1) * (data.m_x_axis_steps-1) * 2;
//    // This vector will store triangles as sets of 3 indices
//    std::vector<GLuint> indices(tri_count*3);
//    uint index = 0;

//    uint index00, index01, index11, index10;
//    int x_step = 0;
////    for(int x_step=0; x_step < data.m_x_axis_steps-1; x_step++){
//        for(int z_step=0; z_step < data.m_z_axis_steps-1; z_step++){
//            index00 = data.data_index(x_step, z_step);
//            index01 = data.data_index(x_step, z_step+1);
//            index11 = data.data_index(x_step+1, z_step+1);
//            index10 = data.data_index(x_step+1, z_step);
//            indices[index+0] = index00;
//            indices[index+1] = index01;
//            indices[index+2] = index10;
////            indices[index+3] = index01;
////            indices[index+4] = index11;
////            indices[index+5] = index10;
//            index += 6;
//        }
////    }

    const int axis_steps = 37;
    std::vector<GLfloat> flat_verts(3 * axis_steps * axis_steps);

    int index = 0;
    for(int z_step=0; z_step < axis_steps; z_step++){
        float z_angle = degToRad( (z_step * 180.0 / (axis_steps-1)) - 90.0);

        for(int x_step=0; x_step < axis_steps; x_step++){
            int vect_index = 3 * index;
            float x_angle = degToRad( (x_step * 360.0 / (axis_steps-1)) - 180.0);
            float x_theta = sin(z_angle);
            float y_theta = cos(z_angle);
            float x_phi = cos(x_angle);
            float z_phi = sin(x_angle);
            flat_verts[vect_index] =  y_theta * x_phi;
            flat_verts[vect_index+1] = x_theta * x_phi;
            flat_verts[vect_index+2] = z_phi;
            index++;
        }
    }

    int tri_count = (axis_steps) * (axis_steps) * 2;
    // This vector will store triangles as sets of 3 indices
    std::vector<GLuint> indices(tri_count*3);
    index = 0;

    uint index00, index01, index11, index10;
    index = 0;
    for(int x_step=0; x_step < axis_steps; x_step++){
        for(int z_step=0; z_step < axis_steps-1; z_step++){
            index00 = (z_step)  +((x_step)  *axis_steps);
            index01 = (z_step+1)+((x_step)  *axis_steps);
            index11 = (z_step+1)+((x_step+1)*axis_steps);
            index10 = (z_step)  +((x_step+1)*axis_steps);
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index += 6;
        }
    }

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices));
    QString name = "ant_vis" + data.index();
    emit built_mesh(mesh, "solid", QColor(255,0,0,128), 10+data.index(), name);
}
