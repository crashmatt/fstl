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
    , tilt(0)
    , yaw(0)
    , perspective(0.0)
    , anim(this, "perspective")
    , status(" ")
    , roll(0)
    , ref_index(-1)
{
	setFormat(format);
    QFile styleFile(":/qt/style.qss");
    styleFile.open( QFile::ReadOnly );
    setStyleSheet(styleFile.readAll());

    anim.setDuration(100);
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

void Canvas::load_mesh(Mesh* m, const QString& shader_name, const QColor& color, const int show_order, const QString& name)
{
    GLMesh *new_mesh = NULL;
    if(shader_name != "visi"){
        new_mesh = new GLMesh(m);
    } else {
        new_mesh = new GLColorMesh(m);
    }

    QVector3D lower(m->xmin(), m->ymin(), m->zmin());
    QVector3D upper(m->xmax(), m->ymax(), m->zmax());
    center = QVector3D(0.0, 0.0, 0.0);

    double new_scale = 2 / (upper - lower).length();
    if(new_scale < scale){
        scale = new_scale;
    }

    QOpenGLShaderProgram* shader = shader_map.value(shader_name, NULL);

    if( (shader != NULL) && !obj_map.contains(show_order) ){
        GLObject *newobj = new GLObject(new_mesh, shader, color, name);
        obj_map[show_order] = newobj;
        obj_name_map[name] = newobj;
    } else {
        delete(new_mesh);
    }

    update();

    delete m;
}

void Canvas::set_rotation(QVector3D rotation, int index)
{
    tilt = rotation.x();
    roll = rotation.y();
    yaw  = rotation.z();
    ref_index = index;
}

void Canvas::set_object_pos(QString& obj_name, QVector3D& pos)
{
    if(obj_name == "all"){
        foreach(GLObject* obj,  obj_map.values()){
            if(obj != NULL){
                obj->m_offset = pos;
            }
        }
    } else {
        GLObject *obj = obj_name_map.value(obj_name, NULL);
        if(obj == NULL) return;
        obj->m_offset = pos;
    }
}

void Canvas::set_view_pos(QVector3D& pos)
{
    center = pos;
}

void Canvas::set_zoom(float zm)
{
    requested_zoom = zm;
}

void Canvas::set_object_visible(QString& obj_name, bool visible)
{
    QRegExp exp(obj_name);
    foreach(QString name, obj_name_map.keys()){
        if(exp.indexIn(name) != -1){
            GLObject *obj = obj_name_map.value(name);
            obj->m_visible = visible;
        }
    }
}

void Canvas::delete_globject(QString& obj_name)
{
    QRegExp exp(obj_name);
    foreach(QString name, obj_name_map.keys()){
        if(exp.indexIn(name) != -1){
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
    roll = 0;
    tilt = 0;
    yaw = 0;
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

    solid_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/visi.vert");
    solid_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/visi.frag");
    solid_shader.link();
    shader_map["visi"] =  &solid_shader;

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
        if(obj) draw_mesh(obj->m_mesh, obj->m_shaderprog, obj->m_color, obj->m_offset);
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
    QVector3D rotation = QVector3D(tilt, roll, yaw);
    emit antenna_visibility(ref_index, rotation, pick_col[1] , g);


    status = QString("RGB:%1,%2,%3 px:%4 gh:%5 zm:%6 tilt:%7 roll:%8 yaw:%9")
            .arg(pick_col[0],2,16, QChar('0')).toUpper()
            .arg(pick_col[1],2,16, QChar('0')).toUpper()
            .arg(pick_col[2],2,16, QChar('0')).toUpper()
            .arg(px)
            .arg(g)
            .arg(zoom)
            .arg(tilt)
            .arg(roll)
            .arg(yaw)
            ;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(10, height() - 10, status);
    painter.drawLine( (pxwidth-px)/2, (pxheight-px)/2, (pxwidth+px)/2, (pxheight+px)/2);
}

void Canvas::draw_mesh(GLMesh* mesh, QOpenGLShaderProgram* shader, const QColor& color, QVector3D offset)
{
    shader->bind();

    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                shader->uniformLocation("transform_matrix"),
                1, GL_FALSE, transform_matrix(offset).data());
    glUniformMatrix4fv(
                shader->uniformLocation("view_matrix"),
                1, GL_FALSE, view_matrix().data());

    shader->setUniformValue("color", color);

    // Compensate for z-flattening when zooming
    glUniform1f(shader->uniformLocation("zoom"), 1/zoom);

    // Then draw the mesh with that vertex position
    mesh->draw(shader);

    shader->release();
}

QMatrix4x4 Canvas::transform_matrix(QVector3D offset) const
{
    QMatrix4x4 m;
    m.rotate(tilt+90.0, QVector3D(1, 0, 0));
    m.rotate(yaw,  QVector3D(0, 0, 1));
    m.rotate(roll,  QVector3D(0, 1, 0));
    m.scale(-scale, scale, -scale);
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
        yaw = fmod(yaw - d.x(), 360);
        tilt = fmod(tilt - d.y(), 360);
        update();
    } else if (event->buttons() & Qt::RightButton)
    {
        roll = fmod(roll + d.x(), 360);
        update();
    }
//    else if (event->buttons() & Qt::RightButton)
//    {
//        center = transform_matrix().inverted() *
//                 view_matrix().inverted() *
//                 QVector3D(-d.x() / (0.5*width()),
//                            d.y() / (0.5*height()), 0);
//        update();
//    }
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
