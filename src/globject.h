#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QColor>

#include "glmesh.h"

class GLObject : protected QOpenGLFunctions
{
public:
    GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const QColor& color, const QString& obj_name);
    ~GLObject();

    GLMesh *m_mesh;
    QOpenGLShaderProgram *m_shaderprog;
    const QColor    m_color;
    const QString   m_name;
    QVector3D       m_offset;
    QVector3D       m_rotation;
    bool            m_visible;
};

#endif // GLOBJECT_H
