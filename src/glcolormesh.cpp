#include "glcolormesh.h"

GLColorMesh::GLColorMesh(const Mesh* const mesh) : GLMesh(mesh)
{

}

void GLColorMesh::draw(GLuint vp, GLuint vc)
{
    vertices.bind();
    indices.bind();

    glVertexAttribPointer(vp, 3, GL_FLOAT, false, 3*sizeof(float), NULL);
    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    vertices.release();
    indices.release();
}
