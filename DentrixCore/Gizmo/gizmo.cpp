#include "gizmo.h"
#include "glm/gtc/type_ptr.hpp"

Gizmo::Gizmo(QOpenGLFunctions_3_3_Core *gl)
{
    position = glm::vec3(0.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    components.push_back(GizmoComponent(glm::vec3(1.0f, 0.0f, 0.0f), rotation, gl));
    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    components.push_back(GizmoComponent(glm::vec3(0.0f, 1.0f, 0.0f), rotation, gl));
    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    components.push_back(GizmoComponent(glm::vec3(0.0f, 0.0f, 1.0f), rotation, gl));
}

void Gizmo::draw(Shader *shader)
{
    for (GizmoComponent c : components) {
        shader->setVec3("color", c.color.x, c.color.y, c.color.z);
        shader->setMatrix4("model", glm::value_ptr(c.rotation));
        c.draw();
    }
}
