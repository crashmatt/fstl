#include "glmesh.h"
#include "mesh.h"

GLMesh::GLMesh(const Mesh* const mesh)
    : vertices(QOpenGLBuffer::VertexBuffer), indices(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    vertices.create();
    indices.create();

    vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indices.setUsagePattern(QOpenGLBuffer::StaticDraw);

    vertices.bind();
    vertices.allocate(mesh->vertices.data(),
                      mesh->vertices.size() * sizeof(float));
    vertices.release();

    indices.bind();
    indices.allocate(mesh->indices.data(),
                     mesh->indices.size() * sizeof(uint32_t));
    indices.release();
}

void GLMesh::draw(QOpenGLShaderProgram* shader)
{
    vertices.bind();
    indices.bind();

    // Find and enable the attribute location for vertex position
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);

    glVertexAttribPointer(vp, 3, GL_FLOAT, false, 3*sizeof(float), NULL);
    glDrawElements(GL_TRIANGLES, indices.size() / sizeof(uint32_t),
                   GL_UNSIGNED_INT, NULL);

    vertices.release();
    indices.release();

    // Clean up state machine
    glDisableVertexAttribArray(vp);
}
