#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QColor>

#include "glmesh.h"

class ObjectConfig
{
public:
    ObjectConfig();
    ObjectConfig(const QString& name, const QString& shader_name, const QColor& base_color, const int order, bool visible=true);

    void write_config(QJsonObject &json) const;

    const QString   m_shadername;
    const QColor    m_color;
    const QString   m_name;
    const int       m_show_order;
    QVector3D       m_offset;
    QQuaternion     m_rotation;
    bool            m_visible;
};

class GLObject : protected QOpenGLFunctions
{
public:
    GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const ObjectConfig &config);
    ~GLObject();

    GLMesh *m_mesh;
    QOpenGLShaderProgram    *m_shaderprog;
    const ObjectConfig      m_config;
    bool                    m_visible;
    QVector3D               m_offset;
    QQuaternion             m_rotation;
};

#endif // GLOBJECT_H
