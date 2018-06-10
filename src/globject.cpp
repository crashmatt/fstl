#include "globject.h"
#include "glmesh.h"
#include <QOpenGLShaderProgram>


GLObject::GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const QColor& color, const QString& obj_name) :
    QOpenGLFunctions()
  , m_mesh(mesh)
  , m_shaderprog(shaderprog)
  , m_color(color)
  , m_name(obj_name)
  , m_offset(QVector3D(0.0, 0.0, 0.0))
  , m_rotation(QVector3D(0.0, 0.0, 0.0))
  , m_visible(true)
{

}

GLObject::~GLObject()
{
    if(m_mesh != NULL) delete m_mesh;
    m_mesh = NULL;
}
