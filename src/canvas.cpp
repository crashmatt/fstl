#include <QMouseEvent>
#include <QColor>
#include <QVector3D>
#include <cmath>

#include "canvas.h"
#include "backdrop.h"
#include "mesh.h"
#include "glmesh.h"
#include "globject.h"


Canvas::Canvas(const QSurfaceFormat& format, QWidget *parent)
    : QOpenGLWidget(parent)
    , scale(1)
    , zoom(2.0)
    , tilt(90)
    , yaw(0)
    , perspective(0.0)
    , anim(this, "perspective")
    , status(" ")
    ,roll(0)
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
    GLMesh *new_mesh = new GLMesh(m);

    QVector3D lower(m->xmin(), m->ymin(), m->zmin());
    QVector3D upper(m->xmax(), m->ymax(), m->zmax());
    center = QVector3D(0.0, 0.0, 0.0);

    double new_scale = 2 / (upper - lower).length();
    if(new_scale < scale){
        scale = new_scale;
    }

    // Reset other camera parameters
//    zoom = 1;
//    yaw = 0;
//    tilt = 90;

    QOpenGLShaderProgram* shader = shader_map.value(shader_name, NULL);

    if( (shader != NULL) && !obj_map.contains(show_order) ){
        GLObject *newobj = new GLObject(new_mesh, shader, color, name);
        obj_map[show_order] = newobj;
        obj_name_map[name] = newobj;
        if(name == "antenna") newobj->m_offset = QVector3D(0.25, 0.25, 0.25);
    } else {
        delete(new_mesh);
    }

    update();

    delete m;
}

void Canvas::set_rotation(QVector3D rotation)
{
    tilt = rotation.x();
    roll = rotation.y();
    yaw = rotation.z();
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

    antenna_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/antenna.vert");
    antenna_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/antenna.frag");
    antenna_shader.link();
    shader_map["antenna"] =  &antenna_shader;

    backdrop = new Backdrop();
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


    foreach(int show_order, obj_map.keys()){
        GLObject* obj = obj_map[show_order];
        if(obj) draw_mesh(obj->m_mesh, obj->m_shaderprog, obj->m_color, obj->m_offset);
    }

    glFlush();      //Make sure rendering is done
    glFinish();

    unsigned char pick_col[3];
    glReadPixels( width()/2, height()/2, 1, 1, GL_RGB , GL_UNSIGNED_BYTE , pick_col );

    status = QString("R:%1 G:%2 B:%3").arg(pick_col[0]).arg(pick_col[1]).arg(pick_col[2]);

    QColor color = QColor(pick_col[0], pick_col[1], pick_col[2]);
    QVector3D rotation = QVector3D(tilt, roll, yaw);
    emit center_color(color, rotation);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(10, height() - 10, status);
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

    // Find and enable the attribute location for vertex position
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Then draw the mesh with that vertex position
    mesh->draw(vp);

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    shader->release();
}

QMatrix4x4 Canvas::transform_matrix(QVector3D offset) const
{
    QMatrix4x4 m;
    m.rotate(tilt, QVector3D(1, 0, 0));
    m.rotate(yaw,  QVector3D(0, 0, 1));
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
    }
    else if (event->buttons() & Qt::RightButton)
    {
        center = transform_matrix().inverted() *
                 view_matrix().inverted() *
                 QVector3D(-d.x() / (0.5*width()),
                            d.y() / (0.5*height()), 0);
        update();
    }
    mouse_pos = p;
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    // Find GL position before the zoom operation
    // (to zoom about mouse cursor)
    auto p = event->pos();
    QVector3D v(1 - p.x() / (0.5*width()),
                p.y() / (0.5*height()) - 1, 0);
    QVector3D a = transform_matrix().inverted() *
                  view_matrix().inverted() * v;

    if (event->delta() < 0)
    {
        for (int i=0; i > event->delta(); --i)
            zoom *= 1.001;
    }
    else if (event->delta() > 0)
    {
        for (int i=0; i < event->delta(); ++i)
            zoom /= 1.001;
    }

    // Then find the cursor's GL position post-zoom and adjust center.
    QVector3D b = transform_matrix().inverted() *
                  view_matrix().inverted() * v;
    center += b - a;
    update();
}

void Canvas::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
