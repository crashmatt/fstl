#include "globject.h"
#include "glmesh.h"
#include <QOpenGLShaderProgram>
#include <QJsonObject>

ObjectConfig::ObjectConfig()
    : m_name("no_name")
    , m_shadername("no_shader")
    , m_color(QColor(0,0,0,0))
    , m_show_order(-1)
    , m_visible(false)
    , m_offset(0.0, 0.0, 0.0)
    , m_rotation()
{

}

ObjectConfig::ObjectConfig(const QString& name, const QString& shader_name, const QColor& base_color, const int order, bool visible)
    : m_name(name)
    , m_shadername(shader_name)
    , m_color(base_color)
    , m_show_order(order)
    , m_visible(visible)
    , m_offset(0.0, 0.0, 0.0)
    , m_rotation()
{

}
Q_DECLARE_METATYPE(ObjectConfig)


void ObjectConfig::write_config(QJsonObject &json) const
{
    QJsonValue name(m_name);
    json["name"] = name;

    QJsonValue shadername(m_shadername);
    json["shadername"] = shadername;

    QJsonObject colour;
    colour["R"] = QJsonValue(m_color.red());
    colour["G"] = QJsonValue(m_color.green());
    colour["B"] = QJsonValue(m_color.blue());
    json["colour"] = colour;

    QJsonValue showorder(m_show_order);
    json["showorder"] = showorder;

    QJsonValue visible(m_visible);
    json["visible"] = visible;


    QJsonObject offset;
    QJsonValue Xoffset(m_offset.x());
    QJsonValue Yoffset(m_offset.y());
    QJsonValue Zoffset(m_offset.z());
    offset["X"] = Xoffset;
    offset["Y"] = Yoffset;
    offset["Z"] = Zoffset;
    json["offset"] = offset;

    QJsonObject rot;
    QVector4D quat = m_rotation.toVector4D();
    QJsonValue Xval(quat.x());
    QJsonValue Yval(quat.y());
    QJsonValue ZVal(quat.z());
    QJsonValue WVal(quat.w());
    rot["Xval"] = Xval;
    rot["Yval"] = Yval;
    rot["Zval"] = ZVal;
    rot["Wval"] = WVal;
    json["rotation"] = rot;
}



GLObject::GLObject(GLMesh *mesh, QOpenGLShaderProgram* shaderprog, const ObjectConfig &config) :
    QOpenGLFunctions()
  , m_mesh(mesh)
  , m_shaderprog(shaderprog)
  , m_config(config)
  , m_visible(config.m_visible)
  , m_offset(config.m_offset)
  , m_rotation(config.m_rotation)
{

}

GLObject::~GLObject()
{
    if(m_mesh != NULL) delete m_mesh;
    m_mesh = NULL;
}
