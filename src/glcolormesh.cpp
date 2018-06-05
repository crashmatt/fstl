#include "glcolormesh.h"

GLColorMesh::GLColorMesh(const Mesh* const mesh) : GLMesh(mesh)
{

}

void GLColorMesh::draw(GLuint vp, GLuint vc)
{
    vertices.bind();
    indices.bind();

    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));

    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    vertices.release();
    indices.release();
}
