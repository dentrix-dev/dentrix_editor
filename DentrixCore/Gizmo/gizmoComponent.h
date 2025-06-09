#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>

enum ComponentAxis {
    X,
    Y,
    Z
};

class GizmoComponent : protected QOpenGLFunctions_3_3_Core
{
    unsigned int VAO, VBO, EBO;
    ComponentAxis axis;

    // TODO: Move buffer data to geometry class
public:
    glm::vec3 color;
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
    glm::mat4 rotation;

public:
    GizmoComponent(glm::vec3 color, glm::mat4 rotation, ComponentAxis axis);
    void draw();
    void onDrag(float xOffset, float yOffset, glm::mat4 &target);
};
