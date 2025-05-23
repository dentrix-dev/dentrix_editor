#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>

class GizmoComponent
{
    QOpenGLFunctions_3_3_Core *gl;
    unsigned int VAO, VBO, EBO;

public:
    glm::vec3 color;
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
    glm::mat4 rotation;

public:
    GizmoComponent(glm::vec3 color, QOpenGLFunctions_3_3_Core *gl);
    void draw();
};
