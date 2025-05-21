#ifndef GIZMO_H
#define GIZMO_H

#include <vector>

#include "Gizmo/gizmoComponent.h"
#include "glm/ext/vector_float3.hpp"

class gizmo
{
    std::vector<gizmoComponent> components;
    glm::vec3 position;

protected:
    gizmo();

public:
    void draw();
};

#endif  // GIZMO_H
