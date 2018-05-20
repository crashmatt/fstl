#include "globject.h"
#include "glmesh.h"
#include <QOpenGLShaderProgram>


GLObject::GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const QColor& color) :
    QOpenGLFunctions()
  , m_mesh(mesh)
  , m_shaderprog(shaderprog)
  , m_color(color)
{

}

GLObject::~GLObject()
{
    if(m_mesh != NULL) delete m_mesh;
    m_mesh = NULL;
}
