#include <future>
#include <QMap>

#include "radpatternloader.h"
#include "vertex.h"

#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)


RadPatternLoader::RadPatternLoader(QObject* parent, const QString& filename, const QString& obj_name, const QString& frag_shader, const QColor& color, int order)
    : QThread(parent)
    , filename(filename)
    , frag_shader(frag_shader)
    , base_color(color)
    , show_order(order)
    , name(obj_name)
{

}

void RadPatternLoader::run()
{
    Mesh* mesh = load_rad_pattern();
    if (mesh)
    {
        emit got_mesh(mesh, frag_shader, base_color, show_order, name);
        emit loaded_file(filename);
    }
}

////////////////////////////////////////////////////////////////////////////////

void RadPatternLoader::parallel_sort(Vertex* begin, Vertex* end, int threads)
{
//    if (threads < 2 || end - begin < 2)
//    {
        std::sort(begin, end);
//    }
//    else
//    {
//        const auto mid = begin + (end - begin) / 2;
//        if (threads == 2)
//        {
//            auto future = std::async(this->parallel_sort, begin, mid, threads / 2);
//            std::sort(mid, end);
//            future.wait();
//        }
//        else
//        {
//            auto a = std::async(std::launch::async, parallel_sort, begin, mid, threads / 2);
//            auto b = std::async(std::launch::async, parallel_sort, mid, end, threads / 2);
//            a.wait();
//            b.wait();
//        }
//        std::inplace_merge(begin, mid, end);
//    }
}

Mesh* RadPatternLoader::mesh_from_verts(uint32_t tri_count, QVector<Vertex>& verts)
{
    // Save indicies as the second element in the array
    // (so that we can reconstruct triangle order after sorting)
    for (size_t i=0; i < tri_count*3; ++i)
    {
        verts[i].i = i;
    }

    // Check how many threads the hardware can safely support. This may return
    // 0 if the property can't be read so we shoud check for that too.
    auto threads = std::thread::hardware_concurrency();
    if (threads == 0)
    {
        threads = 8;
    }

    // Sort the set of vertices (to deduplicate)
    parallel_sort(verts.begin(), verts.end(), threads);

    // This vector will store triangles as sets of 3 indices
    std::vector<GLuint> indices(tri_count*3);

    // Go through the sorted vertex list, deduplicating and creating
    // an indexed geometry representation for the triangles.
    // Unique vertices are moved so that they occupy the first vertex_count
    // positions in the verts array.
    size_t vertex_count = 0;
    for (auto v : verts)
    {
        if (!vertex_count || v != verts[vertex_count-1])
        {
            verts[vertex_count++] = v;
        }
        indices[v.i] = vertex_count - 1;
    }
    verts.resize(vertex_count);

    std::vector<GLfloat> flat_verts;
    flat_verts.reserve(vertex_count*3);
    for (auto v : verts)
    {
        flat_verts.push_back(v.x);
        flat_verts.push_back(v.y);
        flat_verts.push_back(v.z);
    }

    return new Mesh(std::move(flat_verts), std::move(indices));
}

Mesh* RadPatternLoader::load_rad_pattern()
{
    QVector<Vertex*> verts;
    verts.reserve(36*36);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error_missing_file();
        return NULL;
    }

    QVector< QVector<Vertex*>* > totals;
    QList< int > thetas;
    QList< int > phis;

    file.readLine();    //Ignore first two lines as headers
    file.readLine();

    int index  = 0;
    while(!file.atEnd()){
        const auto parts = file.readLine().trimmed().split(',');
        if(parts.count() > 10){
            bool ok;
            float theta = parts[0].toFloat(&ok);
            float phi = parts[1].toFloat(&ok) - 90.0;
//            float ver = parts[2].toFloat(&ok);
//            float hor = parts[3].toFloat(&ok);
            float total = parts[4].toFloat(&ok);
            total = pow(10,total*0.1);
            float x_angle = degToRad(phi);
            float z_angle = degToRad(theta);
            float x_theta = sin(z_angle);
            float y_theta = cos(z_angle);
            float x_phi = cos(x_angle);
            float z_phi = sin(x_angle);
            float radius = total;
            float x =  radius * y_theta * x_phi;
            float y = radius * x_theta * x_phi;
            float z = radius * z_phi;
            verts.append(new Vertex(x, y, z, index));

//            int int_theta = (int) theta;
            int int_phi = (int) phi;
            if(!phis.contains(int_phi)){
                phis.append(int_phi);
                QVector<Vertex*>* newline = new QVector<Vertex*>();
                totals.append( newline );
            }

            totals.last()->append(verts[index]);
            index++;

        }
    }

    if(verts.isEmpty())
        return NULL;

    const int theta_cnt = totals[0]->count();
    const int phi_cnt = totals.count();

    const int vertcount = verts.size();
    std::vector<GLfloat> flat_verts(3*vertcount);
    index = 0;
    for(int i=0; i<vertcount; i++){
        flat_verts[index] = verts[i]->x;
        flat_verts[index+1] = verts[i]->y;
        flat_verts[index+2] = verts[i]->z;
        index += 3;
    }

    int tri_count = (theta_cnt-1) * (phi_cnt-1) * 2;
    std::vector<GLuint> indices(tri_count*3);

    index = 0;
    uint index00, index01, index11, index10;
    for(int t=0; t<(theta_cnt-1); t++){
        for(int p=0; p<(phi_cnt-1); p++){
            index00 = totals[p]->at(t)->i;
            index01 = totals[p]->at(t+1)->i;
            index11 = totals[p+1]->at(t+1)->i;
            index10 = totals[p+1]->at(t)->i;
            indices[index+0] = index00;
            indices[index+1] = index01;
            indices[index+2] = index10;
            indices[index+3] = index01;
            indices[index+4] = index11;
            indices[index+5] = index10;
            index +=6;
        }
    }

    qDeleteAll(totals);
    totals.clear();

    qDeleteAll(verts);
    verts.clear();


//    std::vector<GLfloat> flat_verts(3 * 4);
//    std::vector<GLuint> indices(2*3);

//    uint index = 0;
//    flat_verts[index+0] = -1.0;
//    flat_verts[index+1] = -1.0;
//    flat_verts[index+2] = 0.0;
////    flat_verts[index+3] = 1.0;
////    flat_verts[index+4] = 0.0;
////    flat_verts[index+5] = 0.0;
//    index += 3;
//    flat_verts[index+0] = -1.0;
//    flat_verts[index+1] = 1.0;
//    flat_verts[index+2] = 0.0;
//    index += 3;
//    flat_verts[index+0] = 1.0;
//    flat_verts[index+1] = -1.0;
//    flat_verts[index+2] = 0.0;
//    index += 3;
//    flat_verts[index+0] = 1.0;
//    flat_verts[index+1] = 1.0;
//    flat_verts[index+2] = 0.0;

//    indices[0] = 0;
//    indices[1] = 1;
//    indices[2] = 2;
//    indices[3] = 1;
//    indices[4] = 2;
//    indices[5] = 3;

    Mesh *mesh = new Mesh(std::move(flat_verts), std::move(indices));
    return mesh;
}
