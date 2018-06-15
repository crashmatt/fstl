#include "globject.h"
#include "glmesh.h"
#include <QOpenGLShaderProgram>

ObjectConfig::ObjectConfig()
    : m_name("no_name")
    , m_shadername("no_shader")
    , m_color(QColor(0,0,0,0))
    , m_show_order(-1)
    , m_visible(false)
    , m_offset(0.0, 0.0, 0.0)
    , m_rotation(0.0, 0.0, 0.0)
{

}

ObjectConfig::ObjectConfig(const QString& name, const QString& shader_name, const QColor& base_color, const int order, bool visible)
    : m_name(name)
    , m_shadername(shader_name)
    , m_color(base_color)
    , m_show_order(order)
    , m_visible(visible)
    , m_offset(0.0, 0.0, 0.0)
    , m_rotation(0.0, 0.0, 0.0)
{

}
Q_DECLARE_METATYPE(ObjectConfig)


GLObject::GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const ObjectConfig &config) :
    QOpenGLFunctions()
  , m_mesh(mesh)
  , m_shaderprog(shaderprog)
  , m_color(config.m_color)
  , m_name(config.m_name)
  , m_offset(config.m_offset)
  , m_rotation(config.m_rotation)
  , m_visible(config.m_visible)
{

}

GLObject::~GLObject()
{
    if(m_mesh != NULL) delete m_mesh;
    m_mesh = NULL;
}
