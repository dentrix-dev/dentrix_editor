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
                            glm::vec3& cameraPosition, glm::mat4& view, glm::mat4& projection, glm::ivec4& viewport,
                            glm::mat4& model)
{
    // Plane with which mouse intersection is calculated
    glm::vec4 plane;  // vec4(normal, distance_from_origin)
    // TODO: Improve how each component handles its own tranform logic
    switch (axis) {
        case X:
            plane = glm::vec4(0.0f, 0.0f, 1.0f, objectPosition.x);
            break;
        case Y:
            plane = glm::vec4(1.0f, 0.0f, 0.0f, objectPosition.y);
            break;
        case Z:
            plane = glm::vec4(0.0f, 1.0f, 0.0f, objectPosition.z);
            break;
    }

    // Get old intersection with plane
    glm::vec3 worldPos = glm::unProject(glm::vec3(oldMouseX, oldMouseY, 1.0f), view, projection, viewport);
    glm::vec3 direction = glm::normalize(worldPos - cameraPosition);

    // TODO: Refactor this to Utils::intersectRayWithPlane
    glm::vec3 normal = glm::vec3(plane);
    float denom = glm::dot(normal, direction);

    if (glm::abs(denom) < 1e-6f) return;  // ray is parallel to the plane

    float t = -(glm::dot(normal, cameraPosition) + plane.w) / denom;
    if (t < 0.0f) return;  // intersection is behind the ray origin
    glm::vec3 intersectionPoint = cameraPosition + t * direction;

    // Get new intersection with plane
    worldPos = glm::unProject(glm::vec3(mouseX, mouseY, 1.0f), view, projection, viewport);
    direction = glm::normalize(worldPos - cameraPosition);

    normal = glm::vec3(plane);
    denom = glm::dot(normal, direction);

    if (glm::abs(denom) < 1e-6f) return;  // ray is parallel to the plane

    t = -(glm::dot(normal, cameraPosition) + plane.w) / denom;
    if (t < 0.0f) return;  // intersection is behind the ray origin
    glm::vec3 newIntersectionPoint = cameraPosition + t * direction;

    // Handle transformation
    glm::vec3 delta = newIntersectionPoint - intersectionPoint;
    switch (axis) {
        case X:
            model = glm::translate(model, glm::vec3(delta.x, 0.0f, 0.0f));
            break;
        case Y:
            model = glm::translate(model, glm::vec3(0.0f, -delta.y, 0.0f));
            break;
        case Z:
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, delta.z));
            break;
    }
}
