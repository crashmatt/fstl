#ifndef GLCOLORMESH_H
#define GLCOLORMESH_H

#include "glmesh.h"

class GLColorMesh : public GLMesh
{
public:
    GLColorMesh(const Mesh* const mesh);
    void draw(GLuint vp, GLuint vc);

protected:
    QOpenGLBuffer colors;
};

#endif // GLCOLORMESH_H
