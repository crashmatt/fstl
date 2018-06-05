#ifndef GLMESH_H
#define GLMESH_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

// forward declaration
class Mesh;
class QOpenGLShaderProgram;

class GLMesh : protected QOpenGLFunctions
{
public:
    GLMesh(const Mesh* const mesh);
    virtual void draw(QOpenGLShaderProgram* shader);
protected:
	QOpenGLBuffer vertices;
	QOpenGLBuffer indices;
};

#endif // GLMESH_H
