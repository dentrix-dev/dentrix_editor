#include "gizmo.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Gizmo::Gizmo()
{
    position = glm::vec3(0.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    GizmoComponent *xAxis = new GizmoComponent(glm::vec3(1.0f, 0.0f, 0.0f), rotation);
    components.push_back(xAxis);
    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    GizmoComponent *yAxis = new GizmoComponent(glm::vec3(0.0f, 1.0f, 0.0f), rotation);
    components.push_back(yAxis);
    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    GizmoComponent *zAxis = new GizmoComponent(glm::vec3(0.0f, 0.0f, 1.0f), rotation);
    components.push_back(zAxis);
}

void Gizmo::draw(Shader *shader, float cameraDistance)
{
    for (GizmoComponent *c : components) {
        shader->setVec3("color", c->color.x, c->color.y, c->color.z);
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * c->rotation;
        modelMatrix = glm::scale(modelMatrix, glm::vec3(cameraDistance / 100.0f));
        shader->setMatrix4("model", glm::value_ptr(modelMatrix));
        c->draw();
    }
}
