// Brush.cpp
#include "Brush.h"

Brush::Brush() : radius(1.0f), position(0.0f) {}

void Brush::setRadius(float r) {
    radius = r;
}

float Brush::getRadius(){
    return radius;
}

void Brush::setStrength(float s){
    strength = s;
}

float Brush::getStrength(){
    return strength;
}

void Brush::setPosition(glm::vec3& pos) {
    position = pos;
}

glm::vec3 Brush::getPosition(){
    return position;
}

std::vector<pmp::Vertex> Brush::getVerticesInRadius(pmp::SurfaceMesh& mesh){
    std::vector<pmp::Vertex> verticesInRadius;
    auto vpos = mesh.get_vertex_property<pmp::Point>("v:point");
    if (!vpos) return verticesInRadius;

    for (auto v : mesh.vertices()) {
        pmp::Point p = vpos[v];
        glm::vec3 vertexPos(p[0], p[1], p[2]);
        float distance = glm::distance(vertexPos, position);
        if (distance <= radius)
        {
            verticesInRadius.push_back(v);
        }
    }
    return verticesInRadius;
}
