#ifndef GLMESH_H
#define GLMESH_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

// forward declaration
class Mesh;

class GLMesh : protected QOpenGLFunctions
{
public:
    GLMesh(const Mesh* const mesh);
    virtual void draw(GLuint vp, GLuint vc);
protected:
	QOpenGLBuffer vertices;
	QOpenGLBuffer indices;
};

#endif // GLMESH_H
