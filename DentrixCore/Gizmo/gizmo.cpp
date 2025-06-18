#include "gizmo.h"

#include "Gizmo/gizmoComponent.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Gizmo::Gizmo()
{
    position = glm::vec3(0.0f);

    glm::mat4 rotation = glm::mat4(1.0f);
    enum ComponentAxis axis = X;
    GizmoComponent *xAxis = new GizmoComponent(glm::vec3(0.8f, 0.0f, 0.0f), rotation, axis);
    components.push_back(xAxis);

    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    axis = Y;
    GizmoComponent *yAxis = new GizmoComponent(glm::vec3(0.0f, 0.8f, 0.0f), rotation, axis);
    components.push_back(yAxis);

    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    axis = Z;
    GizmoComponent *zAxis = new GizmoComponent(glm::vec3(0.0f, 0.0f, 0.8f), rotation, axis);
    components.push_back(zAxis);
}

Gizmo::~Gizmo()
{
    for (int i = 0; i < components.size(); i++) delete components[i];
}

void Gizmo::draw(Shader *shader, float cameraDistance, glm::vec3 &cameraPosition, glm::vec3 &cameraForward)
{
    shader->setBool("isFlatColor", true);

    // Calculate distance to render the gizmo at a constant scale
    glm::vec3 cameraToGizmo = position - cameraPosition;
    float distance = glm::dot(cameraToGizmo, cameraForward);
    float scale = distance * 0.01f;
    for (GizmoComponent *c : components) {
        shader->setVec3("color", c->color.x, c->color.y, c->color.z);
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * c->rotation;
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
        shader->setMatrix4("model", glm::value_ptr(modelMatrix));
        c->draw();
    }
}
