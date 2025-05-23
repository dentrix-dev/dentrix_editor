#pragma once

#include <QOpenGLFunctions_3_3_Core>

#include "glm/ext/vector_float3.hpp"

class gizmoComponent
{
    QOpenGLFunctions_3_3_Core* gl;
    unsigned int VAO, VBO, EBO;

public:
    glm::vec3 color;
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;

public:
    gizmoComponent(glm::vec3 color, QOpenGLFunctions_3_3_Core* gl);
    void draw();
};
