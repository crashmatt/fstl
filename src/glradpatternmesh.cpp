#include "glradpatternmesh.h"
#include "mesh.h"

GLRadPatternMesh::GLRadPatternMesh(const Mesh* const mesh) : GLMesh(mesh)
{

}

void GLRadPatternMesh::draw(QOpenGLShaderProgram* shader)
{
    // Find and enable the attribute location for vertex position
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    // Find and enable the attribute location for color position
    const GLuint vc = shader->attributeLocation("vertex_color");
    glEnableVertexAttribArray(vc);

    // Find and enable the attribute location for color position
    const GLuint vertical = shader->attributeLocation("vertical");
    glEnableVertexAttribArray(vertical);

    // Find and enable the attribute location for color position
    const GLuint horizontal = shader->attributeLocation("horizontal");
    glEnableVertexAttribArray(horizontal);

    vertices.bind();
    indices.bind();

    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          12 * sizeof(GLfloat), 0);
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          12 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(vertical, 3, GL_FLOAT, false,
                          12 * sizeof(GLfloat),
                          (GLvoid*)(6 * sizeof(GLfloat)));
    glVertexAttribPointer(horizontal, 3, GL_FLOAT, false,
                          12 * sizeof(GLfloat),
                          (GLvoid*)(9 * sizeof(GLfloat)));

    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    indices.release();
    vertices.release();

    // Clean up state machine
    glDisableVertexAttribArray(vp);
    glDisableVertexAttribArray(vc);
    glDisableVertexAttribArray(vertical);
    glDisableVertexAttribArray(horizontal);
}
