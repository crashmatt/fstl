#include "glcolormesh.h"
#include "mesh.h"

GLColorMesh::GLColorMesh(const Mesh* const mesh) : GLMesh(mesh),
    colors(QOpenGLBuffer::VertexBuffer)
{
    initializeOpenGLFunctions();

    colors.create();

    colors.setUsagePattern(QOpenGLBuffer::StaticDraw);

    colors.bind();
    colors.allocate(mesh->colors.data(),
                      mesh->colors.size() * sizeof(float));
    colors.release();
}

void GLColorMesh::draw(GLuint vp, GLuint vc)
{

    vertices.bind();
    glVertexAttribPointer(vp, 3, GL_FLOAT, false,
                          3 * sizeof(GLfloat), 0);
    vertices.release();

    colors.bind();
    glVertexAttribPointer(vc, 3, GL_FLOAT, false,
                          3 * sizeof(GLfloat), 0);
    colors.release();

    indices.bind();
    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    indices.release();
}
