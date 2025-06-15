#ifndef GIZMO_H
#define GIZMO_H

#include <vector>

#include "Gizmo/gizmoComponent.h"
#include "glm/ext/vector_float3.hpp"
#include "shader.h"

class Gizmo
{
public:
    glm::vec3 position;
    std::vector<GizmoComponent*> components;

    Gizmo();
    void draw(Shader* shader, float cameraDistance, glm::vec3& cameraPosition, glm::vec3& cameraForward);
};

#endif  // GIZMO_H
