#pragma once

#include <glm/glm.hpp>
#include "pmp/surface_mesh.h"

class Brush {
public:
    Brush();

    void setRadius(float r);
    float getRadius();
    void setStrength(float s);
    float getStrength();

    void setPosition(glm::vec3& pos);
    glm::vec3 getPosition();


    std::vector<pmp::Vertex> getVerticesInRadius(pmp::SurfaceMesh& mesh);

private:
    float radius;
    float strength;
    glm::vec3 position;
};
