#ifndef CANVAS_H
#define CANVAS_H

#include <QtOpenGL>
#include <QSurfaceFormat>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <list>

class GLMesh;
class Mesh;
class Backdrop;
class GLObject;
class QColor;

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
	explicit Canvas(const QSurfaceFormat& format, QWidget* parent=0);
    ~Canvas();

    void view_orthographic();
    void view_perspective();

public slots:
    void set_status(const QString& s);
    void clear_status();
    void load_mesh(Mesh* m, const QString& shader, const QColor& color, const int show_order, const QString& name);
    void set_rotation(QVector3D rotation, int ref_index);
    void set_object_pos(QString& obj_name, QVector3D& pos);
    void set_view_pos(QVector3D& pos);
    void set_zoom(float zm);
    void set_object_visible(QString& obj_name, bool visible);
    void delete_globject(QString& obj_name);
    void reset_rotation();

signals:
    void antenna_visibility(int index, QVector3D rotation, float center_visibility , float color_visibility);

protected:
	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
	void set_perspective(float p);
    void view_anim(float v);

private:
    void draw_obj(GLObject* gl_obj);

    QMatrix4x4 transform_matrix(QVector3D offset = QVector3D(0.0, 0.0, 0.0)) const;
    QMatrix4x4 view_matrix() const;

    QOpenGLShaderProgram mesh_shader;
    QOpenGLShaderProgram quad_shader;
    QOpenGLShaderProgram solid_shader;
    QOpenGLShaderProgram antenna_shader;

    QMap<int, GLObject*> obj_map;
    QMap<QString, GLObject*> obj_name_map;
    QMap<QString, QOpenGLShaderProgram*> shader_map;

    Backdrop* backdrop;

    QVector3D center;
    float scale;
    float requested_zoom;
    float zoom;
    float tilt;
    float yaw;
    float roll;
    int   ref_index;

    float perspective;
    Q_PROPERTY(float perspective WRITE set_perspective);
    QPropertyAnimation anim;

    QPoint mouse_pos;
    QString status;
};

#endif // CANVAS_H
