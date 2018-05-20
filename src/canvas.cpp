#include <QMouseEvent>
#include <QColor>

#include <cmath>

#include "canvas.h"
#include "backdrop.h"
#include "mesh.h"
#include "glmesh.h"
#include "globject.h"


Canvas::Canvas(const QSurfaceFormat& format, QWidget *parent)
    : QOpenGLWidget(parent)
    ,scale(1)
    , zoom(1)
    , tilt(90)
    , yaw(0)
    , perspective(0.25)
    , anim(this, "perspective")
    , status(" ")
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

    foreach(GLMesh* mesh, mesh_list){
        if(mesh != NULL) delete mesh;
    }
    mesh_list.clear();

//    foreach(GLObject* obj, obj_list){
//        if(obj != NULL) delete obj;
//    }
//    obj_list.clear();


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

void Canvas::load_mesh(Mesh* m, const QString& shader, const QColor& color)
{
    GLMesh *new_mesh = new GLMesh(m);

    QVector3D lower(m->xmin(), m->ymin(), m->zmin());
    QVector3D upper(m->xmax(), m->ymax(), m->zmax());
    center = QVector3D(0.0, 0.0, 0.0);

    double new_scale = 2 / (upper - lower).length();
    if(new_scale > scale) scale = new_scale;

    // Reset other camera parameters
    zoom = 1;
    yaw = 0;
    tilt = 90;

    GLObject *newobj = new GLObject(new_mesh, &mesh_shader, color);
    obj_list.push_back(newobj);

//    mesh_list.push_back(new_mesh);

    update();

    delete m;
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

    backdrop = new Backdrop();
}


void Canvas::paintGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST | GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	backdrop->draw();

//    foreach(GLMesh* mesh, mesh_list){
//        if(mesh) draw_mesh(mesh, &mesh_shader);
//    }

    foreach(GLObject* obj, obj_list){
        if(obj) draw_mesh(obj->m_mesh, &mesh_shader, obj->m_color);
    }

//    if (m_mesh)  draw_mesh(m_mesh);

	if (status.isNull())  return;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.drawText(10, height() - 10, status);
}

void Canvas::draw_mesh(GLMesh* mesh, QOpenGLShaderProgram* shader, const QColor& color)
{
    shader->bind();

    // Load the transform and view matrices into the shader
    glUniformMatrix4fv(
                shader->uniformLocation("transform_matrix"),
                1, GL_FALSE, transform_matrix().data());
    glUniformMatrix4fv(
                shader->uniformLocation("view_matrix"),
                1, GL_FALSE, view_matrix().data());

//    shader->setUniformValue("color", QColor(200,50,50,128));
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

QMatrix4x4 Canvas::transform_matrix() const
{
    QMatrix4x4 m;
    m.rotate(tilt, QVector3D(1, 0, 0));
    m.rotate(yaw,  QVector3D(0, 0, 1));
    m.scale(-scale, scale, -scale);
    m.translate(-center);
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
