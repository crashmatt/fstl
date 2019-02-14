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
class ObjectConfig;
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

    void write_config(QJsonObject &json) const;

public slots:
    void set_status(const QString& s);
    void set_data_strings(const QStringList& s);
    void clear_status();
    void load_mesh(Mesh* m, ObjectConfig &config);
    void set_rotation(QQuaternion view_rotation, int ref_index);
    void set_object_pos(const QString& obj_name, const QVector3D& pos);
    void set_object_rot(const QString& obj_name, const QQuaternion& rot);
    void set_view_pos(const QVector3D& pos);
    void set_zoom(float zm);
    void set_object_visible(const QString& obj_name, bool visible);
    void delete_globject(const QString& obj_name);
    void reset_rotation();

signals:
    void antenna_visibility(int index, QQuaternion view_rotation, float center_visibility , float color_visibility);
    void loaded_object(ObjectConfig& obj);
    void deleted_object(ObjectConfig& obj);

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

    QList<GLObject*> get_objs(const QString& obj_name);

private:
    void draw_obj(GLObject* gl_obj);

    QMatrix4x4 transform_matrix( QVector3D offset = QVector3D(0.0, 0.0, 0.0), QQuaternion view_rotation = QQuaternion(0.0, 0.0, 0.0, 0.0) ) const;
    QMatrix4x4 view_matrix() const;

    QOpenGLShaderProgram mesh_shader;
    QOpenGLShaderProgram quad_shader;
    QOpenGLShaderProgram solid_shader;
    QOpenGLShaderProgram antenna_shader;
    QOpenGLShaderProgram visi_shader;
    QOpenGLShaderProgram radpattern_shader;

    QMap<int, GLObject*> obj_map;
    QMap<QString, GLObject*> obj_name_map;
    QMap<QString, QOpenGLShaderProgram*> shader_map;

    Backdrop* backdrop;

    QVector3D center;
    float scale;
    float requested_zoom;
    float zoom;
    QQuaternion view_rotation;
    int   ref_index;
    float fps;
    uint  fps_count;
    QTime fps_time;

    float perspective;
    Q_PROPERTY(float perspective WRITE set_perspective);
    QPropertyAnimation anim;

    QPoint      mouse_pos;
    QString     status;
    QStringList data;
};

#endif // CANVAS_H
