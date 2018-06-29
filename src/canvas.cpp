#include <QMouseEvent>
#include <QColor>
#include <QVector3D>
#include <cmath>

#include "canvas.h"
#include "backdrop.h"
#include "mesh.h"
#include "glmesh.h"
#include "glcolormesh.h"
#include "globject.h"


Canvas::Canvas(const QSurfaceFormat& format, QWidget *parent)
    : QOpenGLWidget(parent)
    , scale(1)
    , zoom(2.0)
    , requested_zoom(2.0)
    , view_rotation()
    , perspective(0.0)
    , anim(this, "perspective")
    , status(" ")
    , ref_index(-1)
    , fps(0)
    , fps_count(0)
{
	setFormat(format);
    QFile styleFile(":/qt/style.qss");
    styleFile.open( QFile::ReadOnly );
    setStyleSheet(styleFile.readAll());

    anim.setDuration(100);
    fps_time.start();
}

Canvas::~Canvas()
{
    makeCurrent();

    foreach(GLObject* obj, obj_map.values()){
        if(obj != NULL) delete obj;
    }
    obj_map.clear();


	doneCurrent();
}

void Canvas::view_anim(float v)
{
    anim.setStartValue(perspective);
    anim.setEndValue(v);
    anim.start();
}

void Canvas::view_orthographic()
{
    view_anim(0);
}

void Canvas::view_perspective()
{
    view_anim(0.25);
}

void Canvas::load_mesh(Mesh* m, const ObjectConfig &config)
{
    GLMesh *new_mesh = NULL;
    if(config.m_shadername != "visi"){
        new_mesh = new GLMesh(m);
    } else {
        new_mesh = new GLColorMesh(m);
    }

//    if(config.m_show_order == -1){

//    }

    QVector3D lower(m->xmin(), m->ymin(), m->zmin());
    QVector3D upper(m->xmax(), m->ymax(), m->zmax());
    center = QVector3D(0.0, 0.0, 0.0);

    double new_scale = 2 / (upper - lower).length();
    if(new_scale < scale){
        scale = new_scale;
    }

    QOpenGLShaderProgram* shader = shader_map.value(config.m_shadername, NULL);

    if( (shader != NULL) && !obj_map.contains(config.m_show_order) ){
        GLObject *newobj = new GLObject(new_mesh, shader, config);
        obj_map[config.m_show_order] = newobj;
        obj_name_map[newobj->m_name] = newobj;
        emit loaded_object(newobj->m_name);
    } else {
        delete(new_mesh);
    }

    update();

    delete m;
}

void Canvas::set_rotation(QQuaternion rotation, int index)
{
    Canvas::view_rotation = rotation;
    ref_index = index;
}

QList<GLObject*> Canvas::get_objs(const QString& obj_name)
{
    QList<GLObject*> list;
    if(obj_name == "all"){
        list = obj_map.values();
    } else {
        QRegExp exp(obj_name);
        foreach(QString name, obj_name_map.keys()){
            if(exp.indexIn(name) != -1){
                list.append(obj_name_map[name]);
            }
        }
    }
    return list;
}

void Canvas::set_object_pos(const QString& obj_name, const QVector3D& pos)
{
    QList<GLObject*> list = get_objs(obj_name);
    foreach(GLObject* obj, list){
        obj->m_offset = pos;
    }
}

void Canvas::set_object_rot(const QString& obj_name, const QQuaternion& rot)
{
    QList<GLObject*> list = get_objs(obj_name);
    foreach(GLObject* obj, list){
        obj->m_rotation = rot;
    }
}

void Canvas::set_view_pos(const QVector3D& pos)
{
    center = pos;
}

void Canvas::set_zoom(float zm)
{
    requested_zoom = zm;
}

void Canvas::set_object_visible(const QString& obj_name, bool visible)
{
    QList<GLObject*> list = get_objs(obj_name);
    foreach(GLObject* obj, list){
        obj->m_visible = visible;
    }
}

void Canvas::delete_globject(const QString& obj_name)
{
    QRegExp exp(obj_name);
    foreach(QString name, obj_name_map.keys()){
        auto index = exp.indexIn(name);
        if(index != -1){
            GLObject *obj = obj_name_map.value(name);
            int index = obj_map.key(obj);
            obj_map.remove(index);
            obj_name_map.remove(name);
            delete obj;
        }
    }
}


void Canvas::reset_rotation()
{
    view_rotation = QQuaternion();
    update();
}

void Canvas::set_status(const QString &s)
{
    status = s;
    update();
}

void Canvas::set_perspective(float p)
{
    perspective = p;
    update();
}

void Canvas::clear_status()
{
    status = "";
    update();
}

void Canvas::initializeGL()
{
    initializeOpenGLFunctions();

    mesh_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/mesh.vert");
    mesh_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/mesh.frag");
    mesh_shader.link();
    shader_map["mesh"] = &mesh_shader;

    solid_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/solid.vert");
    solid_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/solid.frag");
    solid_shader.link();
    shader_map["solid"] =  &solid_shader;

    visi_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/visi.vert");
    visi_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/visi.frag");
    visi_shader.link();
    shader_map["visi"] =  &visi_shader;

//    antenna_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/antenna.vert");
//    antenna_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/antenna.frag");
//    antenna_shader.link();
//    shader_map["antenna"] =  &antenna_shader;

    backdrop = new Backdrop(QColor(50,0,50));
}


void Canvas::paintGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);                                //To make sure that both sides of faces are shows with transparency
    glEnable(GL_BLEND);                                     //To support transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      //To support transparency

	backdrop->draw();

    int zoom_step = floor(log(requested_zoom)/(log(2)) + 0.5);
    zoom = pow(2.0, zoom_step);

    foreach(int show_order, obj_map.keys()){
        GLObject* obj = obj_map[show_order];
        if(obj->m_visible){
            if(obj) draw_obj(obj);
        }
    }

    glFlush();      //Make sure rendering is done
    glFinish();

    unsigned char pick_col[3];
    int pxwidth = width();
    int pxheight = height();

    glReadPixels( pxwidth/2, pxheight/2, 1, 1, GL_RGB , GL_UNSIGNED_BYTE , pick_col );

    float px_offset = 0.1 * scale * zoom * 0.5;
    if(pxwidth > pxheight)
        px_offset *= pxwidth;
    else
        px_offset *= pxheight;

    int px = (int) px_offset;
    if(px < 4)
        px = 4;
    if(px > pxwidth)
        px = pxwidth;
    if(px > pxheight)
        px = pxheight;
    const int pxcnt = (px+2)*(px+2);

    unsigned char img[pxcnt];
    glReadPixels( (pxwidth-px)/2, (pxheight-px)/2, px, px, GL_GREEN , GL_UNSIGNED_BYTE , &img[0]);
    long int g=0;
    for(int i=0; i<pxcnt; i++){
        g += img[i];
    }
    g /= pxcnt;

//    QColor color = QColor(pick_col[0], pick_col[1], pick_col[2]);
    emit antenna_visibility(ref_index, view_rotation, pick_col[1] , g);

    int elapsed = fps_time.elapsed();
    if(elapsed > 1000){
        fps = fps_count * 1000.0 / (float) elapsed;
        fps_time.start();
        fps_count = 0;
    } else {
        fps_count++;
    }

    QVector3D euler = view_rotation.toEulerAngles();

    status = QString("RGB:%1,%2,%3 px:%4 gh:%5 zm:%6 x:%7 y:%8 z:%9 fps:%10")
            .arg(pick_col[0],2,16, QChar('0')).toUpper()
            .arg(pick_col[1],2,16, QChar('0')).toUpper()
            .arg(pick_col[2],2,16, QChar('0')).toUpper()
            .arg(px)
            .arg(g)
            .arg(zoom)
            .arg(euler.x(), 3, 'f', 0)
            .arg(euler.y(), 3, 'f', 0)
            .arg(euler.z(), 3, 'f', 0)
            .arg(fps)
            ;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(10, height() - 10, status);
    painter.drawLine( (pxwidth-px)/2, (pxheight-px)/2, (pxwidth+px)/2, (pxheight+px)/2);
}

void Canvas::draw_obj(GLObject* gl_obj)
{
    Q_ASSERT(gl_obj != NULL);
    QOpenGLShaderProgram *shader = gl_obj->m_shaderprog;
    GLMesh *mesh = gl_obj->m_mesh;

    shader->bind();

    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                shader->uniformLocation("transform_matrix"),
                1, GL_FALSE, transform_matrix(gl_obj->m_offset, gl_obj->m_rotation).data());
    glUniformMatrix4fv(
                shader->uniformLocation("view_matrix"),
                1, GL_FALSE, view_matrix().data());

    shader->setUniformValue("color", gl_obj->m_color);

    // Compensate for z-flattening when zooming
    glUniform1f(shader->uniformLocation("zoom"), 1/zoom);

    // Find and enable the attribute location for vertex position
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Find and enable the attribute location for color position
    const GLuint vc = shader->attributeLocation("vertex_color");
    glEnableVertexAttribArray(vc);

    // Then draw the mesh with that vertex position
    mesh->draw(vp, vc);

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    glDisableVertexAttribArray(vc);
    shader->release();
}

QMatrix4x4 Canvas::transform_matrix(QVector3D offset, QQuaternion obj_rotation) const
{
    QMatrix4x4 m;
    auto vr = view_rotation;
    vr.setVector(view_rotation.vector() * -1.0);
    auto rot = vr * obj_rotation;
    rot.normalize();
    m.rotate(rot);
    m.scale(scale, scale, scale);
    m.translate(-center + offset);
    return m;
}

QMatrix4x4 Canvas::view_matrix() const
{
    QMatrix4x4 m;
    if (width() > height())
    {
        m.scale(-height() / float(width()), 1, 0.5);
    }
    else
    {
        m.scale(-1, width() / float(height()), 0.5);
    }
    m.scale(zoom, zoom, 1);
    m(3, 2) = perspective;
    return m;
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
    {
        mouse_pos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton ||
        event->button() == Qt::RightButton)
    {
        unsetCursor();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    auto p = event->pos();
    auto d = p - mouse_pos;


    if (event->buttons() & Qt::LeftButton)
    {
        QQuaternion delta_rot =
                (QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, -d.x())) *
                (QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, d.y()));
        view_rotation = view_rotation * delta_rot;
        update();
    } else if (event->buttons() & Qt::RightButton)
    {
        view_rotation  = view_rotation * QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, -d.x());
        update();
    }
    mouse_pos = p;
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    // Find GL position before the zoom operation
    // (to zoom about mouse cursor)
//    auto p = event->pos();
//    QVector3D v(1 - p.x() / (0.5*width()),
//                p.y() / (0.5*height()) - 1, 0);
//    QVector3D a = transform_matrix().inverted() *
//                  view_matrix().inverted() * v;

    if (event->delta() < 0)
    {
        for (int i=0; i > event->delta(); --i)
            requested_zoom /= 1.001;
    }
    else if (event->delta() > 0)
    {
        for (int i=0; i < event->delta(); ++i)
            requested_zoom *= 1.001;
    }

//    // Then find the cursor's GL position post-zoom and adjust center.
//    QVector3D b = transform_matrix().inverted() *
//                  view_matrix().inverted() * v;
//    center += b - a;
    update();
}

void Canvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
