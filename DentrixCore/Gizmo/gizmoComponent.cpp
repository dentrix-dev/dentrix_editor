#include "gizmoComponent.h"

#include "glm/ext/matrix_transform.hpp"

GizmoComponent::GizmoComponent(glm::vec3 color, glm::mat4 rotation, ComponentAxis axis)
{
    initializeOpenGLFunctions();
    this->axis = axis;
    this->rotation = rotation;
    this->color = color;
    aabb_min = glm::vec3(0.5f, -0.5f, -0.5f);
    aabb_max = glm::vec3(6.0f, 0.5f, 0.5f);
    const std::vector<float> vertices = {
        // positions
        0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f,  // Bottom left +Z
        0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // Top left +Z
        6.0f, 0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // Top right +Z
        6.0f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f,  // Bottom right +Z
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  // Bottom left -Z
        0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f,  // Top left -Z
        6.0f, 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f,  // Top right -Z
        6.0f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  // Bottom right -Z
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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void GizmoComponent::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36 * sizeof(float), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GizmoComponent::onDrag(float xOffset, float yOffset, glm::mat4& target)
{
    switch (axis) {
        case X:
            target = glm::translate(target, glm::vec3(xOffset, 0.0f, 0.0f));
            break;
        case Y:
            target = glm::translate(target, glm::vec3(0.0f, yOffset, 0.0f));
            break;
        case Z:
            target = glm::translate(target, glm::vec3(xOffset, yOffset, 0.0f));
            break;
    }
}
