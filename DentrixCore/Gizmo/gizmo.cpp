#include "gizmo.h"

gizmo::gizmo(QOpenGLFunctions_3_3_Core* gl)
{
    position = glm::vec3(0.0f);
    components.push_back(gizmoComponent(gl));
}

void gizmo::draw()
{
    for (gizmoComponent c : components) c.draw();
}
