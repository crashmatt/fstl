#ifndef GLOBJECT_H
#define GLOBJECT_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "glmesh.h"

class GLObject : public QOpenGLFunctions
{
public:
    GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog);
    ~GLObject();

    GLMesh *m_mesh;
    QOpenGLShaderProgram *m_shaderprog;
};

#endif // GLOBJECT_H
