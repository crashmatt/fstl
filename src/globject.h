#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QColor>

#include "glmesh.h"

class GLObject : protected QOpenGLFunctions
{
public:
    GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const QColor& color);
    ~GLObject();

    GLMesh *m_mesh;
    QOpenGLShaderProgram *m_shaderprog;
    const QColor m_color;
};

#endif // GLOBJECT_H
