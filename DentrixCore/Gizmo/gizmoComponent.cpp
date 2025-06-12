#include "gizmoComponent.h"

#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"

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

void GizmoComponent::onDrag(float oldMouseX, float oldMouseY, float mouseX, float mouseY, glm::vec3& objectPosition,
                            glm::vec3& cameraPosition, glm::vec3& cameraForward, glm::mat4& view, glm::mat4& projection,
                            glm::ivec4& viewport, glm::mat4& model)
{
    glm::vec3 dragAxis;
    switch (axis) {
        case X:
            dragAxis = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case Y:
            dragAxis = glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case Z:
            dragAxis = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
    }

    // Plane perpendicular to the drag axis
    // The mouse ray goes through the object, and is "caught" by the plane
    // The mouse delta on the plane is projected onto the drag axis
    glm::vec3 planeNormal = glm::normalize(glm::cross(glm::cross(dragAxis, cameraForward), dragAxis));

    // Ray-plane intersection for old mouse position
    glm::vec3 rayOld = glm::unProject(glm::vec3(oldMouseX, oldMouseY, 1.0f), view, projection, viewport);
    glm::vec3 dirOld = glm::normalize(rayOld - cameraPosition);

    float denom = glm::dot(planeNormal, dirOld);
    if (glm::abs(denom) < 1e-6f) return;

    float tOld = glm::dot(planeNormal, (objectPosition - cameraPosition)) / denom;
    glm::vec3 intersectionOld = cameraPosition + tOld * dirOld;

    // Ray-plane intersection for new mouse position
    glm::vec3 rayNew = glm::unProject(glm::vec3(mouseX, mouseY, 1.0f), view, projection, viewport);
    glm::vec3 dirNew = glm::normalize(rayNew - cameraPosition);

    denom = glm::dot(planeNormal, dirNew);
    if (glm::abs(denom) < 1e-6f) return;

    float tNew = glm::dot(planeNormal, (objectPosition - cameraPosition)) / denom;
    glm::vec3 intersectionNew = cameraPosition + tNew * dirNew;

    // 4. Project movement onto the drag axis
    glm::vec3 movement = intersectionNew - intersectionOld;
    float movementAlongAxis = glm::dot(movement, dragAxis);
    glm::vec3 delta = movementAlongAxis * dragAxis * glm::vec3(1.0f, -1.0f, 1.0f);

    // 5. Translate model
    model = glm::translate(model, delta);
}
