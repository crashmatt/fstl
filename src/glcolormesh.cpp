#include <QOpenGLShaderProgram>
#include "glcolormesh.h"

GLColorMesh::GLColorMesh(const Mesh* const mesh) : GLMesh(mesh)
{

}

void GLColorMesh::draw(QOpenGLShaderProgram* shader)
{
    vertices.bind();
    indices.bind();

    // Find and enable the attribute location for vertex position
    const GLuint vp = shader->attributeLocation("vertex_position");
    const GLuint vc = shader->attributeLocation("vertex_color");

    glEnableVertexAttribArray(vp);
    glEnableVertexAttribArray(vc);

    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));

    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    vertices.release();
    indices.release();

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    glDisableVertexAttribArray(vc);
}
