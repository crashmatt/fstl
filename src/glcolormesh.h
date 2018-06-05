#ifndef GLCOLORMESH_H
#define GLCOLORMESH_H

#include "glmesh.h"

class Mesh;

class GLColorMesh : public GLMesh
{
public:
    GLColorMesh(const Mesh* const mesh);
    void draw(QOpenGLShaderProgram* shader);
};

#endif // GLCOLORMESH_H
