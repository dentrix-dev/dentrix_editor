#ifndef GIZMO_H
#define GIZMO_H

#include <QOpenGLFunctions_3_3_Core>
#include <vector>

#include "Gizmo/gizmoComponent.h"
#include "glm/ext/vector_float3.hpp"
#include "shader.h"

class gizmo
{
    std::vector<gizmoComponent> components;
    glm::vec3 position;

public:
    gizmo(QOpenGLFunctions_3_3_Core* gl);
    void draw(Shader* shader);
};

#endif  // GIZMO_H
