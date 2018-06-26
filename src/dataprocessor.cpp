#include "dataprocessor.h"
#include "mesh.h"
#include "antennadata.h"
#include "radpatterndata.h"
#include "globject.h"
#include <QMatrix4x4>

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

DataProcessor::DataProcessor(QObject *parent) : QObject(parent)
{

}

void DataProcessor::process_data(Antenna *antenna)
{
}


//void DataProcessor::process_data(AntennaData &data, Antenna &config)
//{
//    float max_vis = 0.01;
//    foreach(AntennaDataPoint* datapt, data.m_antenna_data){
//        Q_ASSERT(datapt != NULL);
//        if(datapt->m_color_visibility > max_vis){
//            max_vis = datapt->m_color_visibility;
//        }
//    }
//    foreach(AntennaDataPoint* datapt, data.m_antenna_data){
//        datapt->m_center_visibility = datapt->m_center_color.greenF();
//        datapt->m_visibility = datapt->m_color_visibility / max_vis;
//    }

////    build_antenna_visibility_object(data, config);

//    build_antenna_effective_object(data, config);
//}

//void DataProcessor::build_antenna_visibility_object(AntennaData &data, const Antenna &config)
//{
//    std::vector<GLfloat> flat_verts(2 * 3 * data.m_z_axis_steps * data.m_x_axis_steps);

//    QColor viscolor;
//    switch(data.m_index){
//    case 0:
//        viscolor = QColor(255,0,0);
//        break;
//    case 1:
//        viscolor = QColor(0,0,255);
//        break;
//    case 2:
//        viscolor = QColor(0,255,0);
//        break;
//    default:
//        viscolor = QColor(128,128,128);
//        break;
//    }

//    AntennaDataPoint* datapt;
//    int vect_index = 0;
//    foreach(datapt ,data.m_antenna_data){
//        Q_ASSERT(datapt != NULL);
//        auto vect = datapt->m_rotation.inverted().rotatedVector(QVector3D(0.0, 1.0, 0.0)) * datapt->m_visibility;
//        flat_verts[vect_index] =  vect.x();
//        flat_verts[vect_index+1] = vect.y();
//        flat_verts[vect_index+2] = vect.z();
//        float color_scale = datapt->m_visibility * datapt->m_visibility;
//        flat_verts[vect_index+3] = color_scale * viscolor.redF();
//        flat_verts[vect_index+4] = color_scale * viscolor.greenF();
//        flat_verts[vect_index+5] = color_scale * viscolor.blueF();
//        vect_index += 6;
//    }

//    int tri_count = (data.m_z_axis_steps) * (data.m_x_axis_steps) * 2;
//    // This vector will store triangles as sets of 3 indices
//    std::vector<GLuint> indices(tri_count*3);

//    uint index = 0;
//    uint index00, index01, index11, index10;
//    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
//        for(int x_step=0; x_step < data.m_x_axis_steps-1; x_step++){
//            index00 = data.data_index(z_step, x_step);
//            index01 = data.data_index(z_step+1, x_step);
//            index11 = data.data_index(z_step+1, x_step+1);
//            index10 = data.data_index(z_step, x_step+1);
//            indices[index+0] = index00;
//            indices[index+1] = index01;
//            indices[index+2] = index10;
//            indices[index+3] = index01;
//            indices[index+4] = index11;
//            indices[index+5] = index10;
//            index += 6;
//        }
//    }

//    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
//    QString name = QString("ant_vis%1").arg(data.index());

//    auto mesh_config = ObjectConfig(config.m_name, "visi", QColor(255,0,0,128), 20+data.index());
//    emit built_mesh(mesh, mesh_config);
//    emit set_obj_pos(name, config.m_pos);
//}


//void DataProcessor::build_antenna_effective_object(AntennaData &data, const Antenna &config)
//{
//    std::vector<GLfloat> flat_verts(2 * 3 * data.m_z_axis_steps * data.m_x_axis_steps);

//    QColor viscolor;
//    switch(data.m_index){
//    case 0:
//        viscolor = QColor(255,0,0);
//        break;
//    case 1:
//        viscolor = QColor(0,0,255);
//        break;
//    case 2:
//        viscolor = QColor(0,255,0);
//        break;
//    default:
//        viscolor = QColor(128,128,128);
//        break;
//    }

//    auto pattern = m_patterns->get_data(config.m_type).data();
//    Q_ASSERT(pattern != NULL);

//    AntennaDataPoint* datapt;
//    int vect_index = 0;
//    foreach(datapt ,data.m_antenna_data){
//        Q_ASSERT(datapt != NULL);
//        auto intensity = get_rad_intensity(pattern, datapt);
//        auto vect = datapt->m_rotation.inverted().rotatedVector(QVector3D(0.0, 1.0, 0.0)) * intensity;
//        flat_verts[vect_index] =  vect.x();
//        flat_verts[vect_index+1] = vect.y();
//        flat_verts[vect_index+2] = vect.z();
//        float color_scale = datapt->m_visibility * datapt->m_visibility;
//        flat_verts[vect_index+3] = color_scale * viscolor.redF();
//        flat_verts[vect_index+4] = color_scale * viscolor.greenF();
//        flat_verts[vect_index+5] = color_scale * viscolor.blueF();
//        vect_index += 6;
//    }

////    AntennaDataPoint* datapt;
////    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
////        for(int x_step=0; x_step < data.m_x_axis_steps; x_step++){
////            datapt = data.get_antenna_datapoint(x_step, z_step);
////            Q_ASSERT(datapt != NULL);
////            int vect_index = 2 * 3 * data.data_index(z_step, x_step);

////            auto vect = datapt->m_rotation.inverted().rotatedVector(QVector3D(0.0, 1.0, 0.0)) * datapt->m_visibility;

////            float intensity = get_rad_intensity(pattern, vect);

////            vect = QVector4D(0.0, -intensity, 0, 0);
////            vect = vect * rot;


////            flat_verts[vect_index] =  vect.x();
////            flat_verts[vect_index+1] = vect.y();
////            flat_verts[vect_index+2] = vect.z();
////            float color_scale = datapt->m_visibility * datapt->m_visibility;
////            flat_verts[vect_index+3] = color_scale * viscolor.redF();
////            flat_verts[vect_index+4] = color_scale * viscolor.greenF();
////            flat_verts[vect_index+5] = color_scale * viscolor.blueF();
////        }
////    }

//    int tri_count = (data.m_z_axis_steps) * (data.m_x_axis_steps) * 2;
//    // This vector will store triangles as sets of 3 indices
//    std::vector<GLuint> indices(tri_count*3);

//    uint index = 0;
//    uint index00, index01, index11, index10;
//    for(int z_step=0; z_step < data.m_z_axis_steps; z_step++){
//        for(int x_step=0; x_step < data.m_x_axis_steps-1; x_step++){
//            index00 = data.data_index(z_step, x_step);
//            index01 = data.data_index(z_step+1, x_step);
//            index11 = data.data_index(z_step+1, x_step+1);
//            index10 = data.data_index(z_step, x_step+1);
//            indices[index+0] = index00;
//            indices[index+1] = index01;
//            indices[index+2] = index10;
//            indices[index+3] = index01;
//            indices[index+4] = index11;
//            indices[index+5] = index10;
//            index += 6;
//        }
//    }

//    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);

//    auto mesh_config = ObjectConfig(config.m_name, "visi", QColor(255,0,0,128), 20+data.index());
//    emit built_mesh(mesh, mesh_config);
//    emit set_obj_pos(config.m_name, config.m_pos);
//}


//float DataProcessor::get_rad_intensity(RadPatternSet* pattern, AntennaDataPoint* antpt)
//{
//    //find closest measurement
//    RadPatternPoint* pt = pattern->nearest_point(antpt->m_rotation);
//    if(pt == NULL)
//        return 0.0;
//    return pt->get_amplitude();
//}
