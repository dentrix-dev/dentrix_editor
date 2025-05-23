#include "gizmoComponent.h"

GizmoComponent::GizmoComponent(glm::vec3 color, glm::mat4 rotation, QOpenGLFunctions_3_3_Core* gl)
{
    this->rotation = rotation;
    this->color = color;
    this->gl = gl;
    const std::vector<float> vertices = {
        // positions
        0.1f, -0.1f, 0.1f,  1.0f, 1.0f, 1.0f,  // Bottom left +Z
        0.1f, 0.1f,  0.1f,  1.0f, 1.0f, 1.0f,  // Top left +Z
        5.0f, 0.1f,  0.1f,  1.0f, 1.0f, 1.0f,  // Top right +Z
        5.0f, -0.1f, 0.1f,  1.0f, 1.0f, 1.0f,  // Bottom right +Z
        0.1f, -0.1f, -0.1f, 1.0f, 1.0f, 1.0f,  // Bottom left -Z
        0.1f, 0.1f,  -0.1f, 1.0f, 1.0f, 1.0f,  // Top left -Z
        5.0f, 0.1f,  -0.1f, 1.0f, 1.0f, 1.0f,  // Top right -Z
        5.0f, -0.1f, -0.1f, 1.0f, 1.0f, 1.0f,  // Bottom right -Z
    };

    const std::vector<unsigned int> indices = {// front face
                                               0, 1, 2, 2, 3, 0,
                                               // right
                                               1, 5, 6, 6, 2, 1,
                                               // back
                                               5, 4, 7, 7, 6, 5,
                                               // left
                                               4, 0, 3, 3, 7, 4,
                                               // top
                                               3, 2, 6, 6, 7, 3,
                                               // bottom
                                               4, 5, 1, 1, 0, 4};

    gl->glGenVertexArrays(1, &VAO);
    gl->glGenBuffers(1, &VBO);
    gl->glGenBuffers(1, &EBO);

    gl->glBindVertexArray(VAO);

    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    gl->glBindVertexArray(0);
}

void GizmoComponent::draw()
{
    gl->glBindVertexArray(VAO);
    gl->glDrawElements(GL_TRIANGLES, 36 * sizeof(float), GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);
};
