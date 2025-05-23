#include "gizmo.h"
#include "glm/gtc/type_ptr.hpp"

Gizmo::Gizmo(QOpenGLFunctions_3_3_Core *gl)
{
    position = glm::vec3(0.0f);
    components.push_back(GizmoComponent(glm::vec3(1.0f, 0.0f, 0.0f), gl));
}

void Gizmo::draw(Shader *shader)
{
    for (GizmoComponent c : components) {
        shader->setVec3("color", c.color.x, c.color.y, c.color.z);
        shader->setMatrix4("model", glm::value_ptr(c.rotation));
        c.draw();
    }
}
