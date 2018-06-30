#include "glcolormesh.h"
#include "mesh.h"

GLColorMesh::GLColorMesh(const Mesh* const mesh) : GLMesh(mesh)
{
}

void GLColorMesh::draw(QOpenGLShaderProgram* shader)
{
    // Find and enable the attribute location for vertex position
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Find and enable the attribute location for color position
    const GLuint vc = shader->attributeLocation("vertex_color");
    glEnableVertexAttribArray(vc);

    vertices.bind();
    indices.bind();

    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));

    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    indices.release();
    vertices.release();

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    glDisableVertexAttribArray(vc);
}
