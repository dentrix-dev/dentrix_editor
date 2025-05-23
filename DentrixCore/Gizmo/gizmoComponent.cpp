#include "gizmoComponent.h"
#include "glm/ext/matrix_transform.hpp"

GizmoComponent::GizmoComponent(glm::vec3 color, QOpenGLFunctions_3_3_Core* gl)
{
    this->rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f));
    this->color = color;
    this->gl = gl;
    const std::vector<float> vertices = {
        // positions
        -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f,  // 0
        -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // 1
        5.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // 2
        5.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // 3
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  // 4
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  // 5
        5.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 1.0f,  // 6
        5.5f, -0.5f,  -0.5f, 1.0f, 1.0f, 1.0f,  // 7
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

void GizmoComponent::draw() {
    gl->glBindVertexArray(VAO);
    gl->glDrawElements(GL_TRIANGLES, 36 * sizeof(float), GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);
};
