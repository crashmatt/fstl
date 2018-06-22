#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include "radpatterndata.h"
#include "globject.h"
#include <QMatrix4x4>

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

DataProcessor::DataProcessor(QObject *parent, RadPatternData* patterns) : QObject(parent)
  , m_patterns(patterns)

{

}

void DataProcessor::process_data(AntennaData &data, AntennaConfig &config)
{
    float max_vis = 0.01;
    foreach(AntennaDataPoint* datapt, data.m_antenna_data){
        Q_ASSERT(datapt != NULL);
        if(datapt->m_color_visibility > max_vis){
            max_vis = datapt->m_color_visibility;
        }
    }
    foreach(AntennaDataPoint* datapt, data.m_antenna_data){
        datapt->m_center_visibility = datapt->m_center_color.greenF();
        datapt->m_visibility = datapt->m_color_visibility / max_vis;
    }

    build_antenna_visibility_object(data, config);

//    build_antenna_effective_object(data, config);
}

void DataProcessor::build_antenna_visibility_object(AntennaData &data, const AntennaConfig &config)
{
    std::vector<GLfloat> flat_verts(2 * 3 * data.m_z_axis_steps * data.m_x_axis_steps);

    QColor viscolor;
    switch(data.m_index){
    case 0:
        viscolor = QColor(255,0,0);
        break;
    case 1:
        viscolor = QColor(0,0,255);
        break;
    case 2:
        viscolor = QColor(0,255,0);
        break;
    default:
        viscolor = QColor(128,128,128);
        break;
    }

    AntennaDataPoint* datapt;
    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
        for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
            datapt = data.get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            int vect_index = 2 * 3 * data.data_index(z_step, x_step);

            QMatrix4x4 rot;
            rot.rotate(datapt->m_rotation.z(), QVector3D(0.0, 0.0, -1.0));
            rot.rotate(datapt->m_rotation.x(), QVector3D(-1.0, 0.0, 0.0));

            auto vect = QVector4D(0.0, -datapt->m_visibility, 0, 0);
            vect = vect * rot;

            flat_verts[vect_index] =  vect.x();
            flat_verts[vect_index+1] = vect.y();
            flat_verts[vect_index+2] = vect.z();
            float color_scale = datapt->m_visibility * datapt->m_visibility;
            flat_verts[vect_index+3] = color_scale * viscolor.redF();
            flat_verts[vect_index+4] = color_scale * viscolor.greenF();
            flat_verts[vect_index+5] = color_scale * viscolor.blueF();
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

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    QString name = QString("ant_vis%1").arg(data.index());

    auto mesh_config = ObjectConfig(config.m_name, "visi", QColor(255,0,0,128), 20+data.index());
    emit built_mesh(mesh, mesh_config);
    emit set_obj_pos(name, config.m_pos);
}


void DataProcessor::build_antenna_effective_object(AntennaData &data, const AntennaConfig &config)
{
    std::vector<GLfloat> flat_verts(2 * 3 * data.m_z_axis_steps * data.m_x_axis_steps);

    QColor viscolor;
    switch(data.m_index){
    case 0:
        viscolor = QColor(255,0,0);
        break;
    case 1:
        viscolor = QColor(0,0,255);
        break;
    case 2:
        viscolor = QColor(0,255,0);
        break;
    default:
        viscolor = QColor(128,128,128);
        break;
    }

    auto pattern = m_patterns->get_data(config.m_type).data();
    Q_ASSERT(pattern != NULL);

    AntennaDataPoint* datapt;
    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
        for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
            datapt = data.get_antenna_datapoint(x_step, z_step);
            Q_ASSERT(datapt != NULL);
            QVector3D rot = datapt->m_rotation * QVector3D(-1.0, 1.0, 1.0);
            rot += QVector3D(0.0, 0.0, -90.0);
            float x_angle = degToRad(rot.x());
            float z_angle = degToRad(rot.z());
            int vect_index = 2 * 3 * data.data_index(z_step, x_step);
            float x_theta = sin(z_angle);
            float y_theta = cos(z_angle);
            float x_phi = cos(x_angle);
            float z_phi = sin(x_angle);
            float radius = 1.0;

            QVector3D radrot = datapt->m_rotation * QVector3D(1.0, 1.0, 1.0);
            radrot += QVector3D(0.0, 0.0, 0.0);
            float intensity = get_rad_intensity(pattern, datapt->m_rotation);
            radius =  intensity;    //datapt->m_visibility *
//                radius =  datapt->m_visibility;

            flat_verts[vect_index] =  radius * y_theta * x_phi;
            flat_verts[vect_index+1] = radius * x_theta * x_phi;
            flat_verts[vect_index+2] = radius * z_phi;
            float color_scale = datapt->m_visibility * datapt->m_visibility;
            flat_verts[vect_index+3] = color_scale * viscolor.redF();
            flat_verts[vect_index+4] = color_scale * viscolor.greenF();
            flat_verts[vect_index+5] = color_scale * viscolor.blueF();
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

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);

    auto mesh_config = ObjectConfig(config.m_name, "visi", QColor(255,0,0,128), 20+data.index());
    emit built_mesh(mesh, mesh_config);
    emit set_obj_pos(config.m_name, config.m_pos);
}


float DataProcessor::get_rad_intensity(RadPatternSet* pattern, QVector3D rot)
{
    //find closest measurement
    RadPatternPoint* pt = pattern->nearest_point(rot.x(), rot.z());
    if(pt == NULL)
        return 0.0;
    return pt->get_amplitude();
}
