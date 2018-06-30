#ifndef GLRADPATTERNMESH_H
#define GLRADPATTERNMESH_H

#include "glmesh.h"

class GLRadPatternMesh : public GLMesh
{
public:
    GLRadPatternMesh(const Mesh* const mesh);
    void draw(QOpenGLShaderProgram* shader);

protected:
    QOpenGLBuffer verticals;
    QOpenGLBuffer horizontals;
};

#endif // GLRADPATTERNMESH_H
