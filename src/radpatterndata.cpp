#include "radpatterndata.h"
#include "mesh.h"


#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

RadPatternPoint::RadPatternPoint(int i, float t, float p, float v, float h, float tot)
  : theta(t)
  , phi(p)
  , ver(v)
  , hor(h)
  , total(tot)
  , index(i)
  , rot()
{
    rot =   QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, phi) * QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, theta);
};

Vertex RadPatternPoint::make_vertex()
{
    auto vect = rot.rotatedVector(QVector3D(0.0, 0.0, 1.0));
    rad_vect = vect;
    vect *= get_amplitude();
    return Vertex(vect.x(), vect.y(), vect.z(), index);
}

QColor RadPatternPoint::get_color()
{
    QColor color(0,0,0,125);
    color.setRedF( pow(10, ver*0.2 )*0.25);
    color.setGreenF(pow(10, hor*0.2 )*0.25);
    color.setBlueF(1.0-pow(10, total*0.2 )*0.25);
    return color;
}

////////////////////////////////////////////////////////////////////////////////

RadPatternSet::RadPatternSet(QString name)
    : set_name(name)
{

}

RadPatternSet::~RadPatternSet()
{
    //Clear these so referenced contents are not automatically deleted
    angle_index_map.clear();
    id_index_map.clear();
    phis.clear();
    thetas.clear();
}

RadPatternSet::index_id RadPatternSet::get_id(int phi, int theta)
{
    RadPatternSet::index_id id;
    id._.phi = phi;
    id._.theta = theta;
    return id;
}

bool RadPatternSet::build_maps()
{
    angle_index_map.clear();
    id_index_map.clear();
    phis.clear();
    thetas.clear();

    int phi_index = 0;
    int theta_index = 0;
    int pt_index = 0;
    foreach(RadPatternPoint* pt, rad_data){
        const int phi     = (int) pt->phi;
        const int theta   = (int) pt->theta;
        auto id = get_id(phi, theta);
        angle_index_map[id.id] = pt_index;

        if(!phis.contains(phi)){
            phis.append(phi);
            phi_index = phis.size() - 1;
            theta_index = 0;
        } else {
            theta_index++;
        }
        if(!thetas.contains(theta)){
            thetas.append(theta);
        }

        id = get_id(phi_index, theta_index);
        id_index_map[id.id] = pt_index;
        pt_index++;
    }

    auto total = phis.count() * thetas.count();
    if(rad_data.count() != total){
        id_index_map.clear();
        angle_index_map.clear();
        phis.clear();
        thetas.clear();
        return false;
    }
    return true;
}


RadPatternPoint* RadPatternSet::get_point(int phi, int theta)
{
    const auto id = get_id(phi, theta);
    return rad_data[angle_index_map.value(id.id, NULL)];
}

int RadPatternSet::get_index(int phi, int theta){
    const auto id = get_id(phi, theta);
    return angle_index_map.value(id.id, -1);
}


//RadPatternPoint* RadPatternSet::get_point_at_index(uint phi_index, uint theta_index)
//{
//    const auto id = get_id(phi_index, theta_index);
//    return rad_data[id_index_map.value(id.id, NULL)];
//}

RadPatternPoint* RadPatternSet::nearest_point(QQuaternion rot)
{
    if(rad_data.isEmpty()) return NULL;
    double nearest_dist = 1000;
    RadPatternPoint* nearest = rad_data[0];

//    auto euler = rot.toEulerAngles();
//    auto target_phi = (int) euler.y();
//    auto target_theta = (int) euler.z();

//    // Adjust legnths for raparound
//    auto phi_count = phis.length() - 1;
//    auto theta_count = thetas.length() - 1;

//    float phi_step_angle = 360.0 / (float) phi_count;
//    float theta_step_angle = 180.0 / (float) theta_count;

//    auto phi_delta = (float) target_phi - phis[0];
//    auto theta_delta = (float) target_theta - thetas[0];

//    auto phi_step = (int) (phi_delta / phi_step_angle);
//    phi_step += phi_count;
//    phi_step = phi_step % phi_count;
//    auto theta_step = (int) (theta_delta / theta_step_angle);
//    theta_step = theta_step % (theta_count+1);

//    auto phi = phis[phi_step];
//    auto theta = thetas[theta_step];

//    auto refpt = get_point(phi, theta);

//    return refpt;

    const auto vect = rot.rotatedVector(QVector3D(0.0, 0.0, 1.0));

    foreach(RadPatternPoint* pt, rad_data){
        auto ptvect = pt->rad_vect;
        auto diffs = (ptvect - vect);
        auto diff =fabs(diffs.length());
        if(diff < nearest_dist){
            nearest_dist = diff;
            nearest = pt;
        }
    }

//    auto phi_low_est_idx = 0;
//    auto phi_high_est_idx = phis.length() - 2;
//    auto theta_low_est_idx = 0;
//    auto theta_high_est_idx = thetas.length() - 1;


//    auto phi_mid_idx = (phi_low_est_idx + phi_low_est_idx) / 2;
//    auto phi_mid = phis[phi_mid_idx];
//    for(auto it=0; it<thetas.length(); it++){
//        auto theta_low = thetas[theta_low_est_idx];
//        auto theta_high = thetas[theta_high_est_idx];
//        auto theta_pt_low = get_point(phi_mid, theta_low);
//        auto theta_pt_high = get_point(phi_mid, theta_high);
//        float len_low = fabs((vect - theta_pt_low->rad_vect).length());
//        float len_high = fabs((vect - theta_pt_high->rad_vect).length());
//        if(len_high > len_low){
//            theta_low_est_idx = (theta_high_est_idx+theta_low_est_idx) / 2.0;
//        } else {
//            theta_high_est_idx = (theta_high_est_idx+theta_low_est_idx) / 2.0;
//        }
//    }

//    //If theta is at the ends then don't try to find theta since all the theta points are at the same position
//    if(theta_low_est_idx == 0){
//        phi_low_est_idx = 0;
//        phi_high_est_idx = 0;
//    } else if(theta_low_est_idx == (thetas.length() - 1)){
//        phi_low_est_idx = 0;
//        phi_high_est_idx = 0;
//    }else {
//        auto theta = thetas[theta_low_est_idx];
//        for(auto it=0; it<phis.length(); it++){

//            auto phi_low = phis[phi_low_est_idx];
//            auto phi_high = phis[phi_high_est_idx];
//            auto phi_pt_low = get_point(phi_low, theta);
//            auto phi_pt_high = get_point(phi_high, theta);
//            auto len_low = fabs((vect - phi_pt_low->rad_vect).length());
//            auto len_high = fabs((vect - phi_pt_high->rad_vect).length());
//            if(len_high > len_low){
//                phi_low_est_idx = (phi_high_est_idx+phi_low_est_idx) / 2.0;
//            } else {
//                phi_high_est_idx = (phi_high_est_idx+phi_low_est_idx) / 2.0;
//            }
//        }
//    }

//    auto est_phi = phis[phi_low_est_idx];
//    auto est_theta = phis[theta_low_est_idx];
//    auto estpt = get_point(est_phi, est_theta);

//    if(estpt != nearest){
//        qWarning() << "Nearest point estimate does not agree. pt:" << estpt << " vs:" << nearest;
//    }

    return nearest;
}


Mesh* RadPatternSet::create_mesh()
{
    if(rad_data.isEmpty())
        return NULL;

    const int vertcount = rad_data.size();
    std::vector<GLfloat> flat_verts(6*vertcount);
    int index = 0;
    for(int i=0; i<vertcount; i++){
        Vertex vertex = rad_data[i]->make_vertex();
        QColor color = rad_data[i]->get_color();
        flat_verts[index] = vertex.x;
        flat_verts[index+1] = vertex.y;
        flat_verts[index+2] = vertex.z;
        flat_verts[index+3] = color.redF();
        flat_verts[index+4] = color.greenF();
        flat_verts[index+5] = color.blueF();
        index += 6;
    }

    std::vector<GLuint> indices;
    make_indices(indices);

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices), 6);
    return mesh;
}


void RadPatternSet::make_indices(std::vector<GLuint>& indices)
{
    const int theta_cnt = thetas.size();
    const int phi_cnt = phis.size();
    int tri_count = (theta_cnt-1) * (phi_cnt-1) * 2;

    indices.resize(tri_count*3);

    int index = 0;
    uint index00, index01, index11, index10;
    for(int p=0; p<(phi_cnt-1); p++){
        for(int t=0; t<(theta_cnt-1); t++){
            index00 = id_index_map.value(get_id(p, t).id, 0);
            index01 = id_index_map.value(get_id(p, t+1).id, 0);
            index11 = id_index_map.value(get_id(p+1, t+1).id, 0);
            index10 = id_index_map.value(get_id(p+1, t).id, 0);
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index +=6;
        }
    }
}

void RadPatternSet::write(QJsonObject &json) const
{
}

////////////////////////////////////////////////////////////////////////////////

RadPatternData* RadPatternData::s_instance = NULL;

RadPatternData::RadPatternData(QObject *parent) : QObject(parent)
{
    s_instance = this;
}

QExplicitlySharedDataPointer<RadPatternSet> RadPatternData::get_data(const QString &pattern_name)
{
    return m_pattern_data.value( pattern_name, QExplicitlySharedDataPointer<RadPatternSet>() );
}

void RadPatternData::add_pattern_data(RadPatternSet* data)
{
    Q_ASSERT(data != NULL);
    if(m_pattern_data.contains(data->set_name)){
        m_pattern_data.remove(data->set_name);
    }
    m_pattern_data[data->set_name] = data;
}

void RadPatternData::write(QJsonObject &json) const
{
//    QJsonArray patterns;

//    foreach (const auto pattern, pattern_data.values()) {
//        QJsonObject patternObject;
//        pattern.write(patternObject);
//        patterns.append(patternObject);
//    }
//    json["levels"] = levelArray;
}

QDataStream &operator<<(QDataStream &, const RadPatternData &)
{

}

QDataStream &operator>>(QDataStream &, RadPatternData &){

}
