#include "globject.h"

GLObject::GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog) :
    QOpenGLShaderProgram()
  , m_mesh(mesh)
  , m_shaderprog(shaderprog)
{

}

GLObject::~GLObject()
{
    if(m_mesh != NULL) delete m_mesh;
    m_mesh = NULL;
}
