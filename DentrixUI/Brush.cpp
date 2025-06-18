// Brush.cpp
#include "Brush.h"
#include <iostream>

Brush::Brush() : radius(10.0f), strength(10), position(0.0f) {}

void Brush::setRadius(float r) {
    radius = r;
}

float Brush::getRadius(){
    return radius;
}

void Brush::setStrength(int s){
    strength = s;
}

int Brush::getStrength(){
    return strength;
}

void Brush::setPosition(glm::vec3& pos) {
    position = pos;
}

glm::vec3 Brush::getPosition(){
    return position;
}

std::vector<pmp::Vertex> Brush::getVerticesInRadius(const pmp::SurfaceMesh& mesh) {
    std::vector<pmp::Vertex> verticesInRadius;
    if (!kdtree_built) {
        kdtree = std::make_unique<mcg::Mesh_KDTree>(mcg::Mesh_KDTree::build(mesh, 16));
        kdtree_built = true;
    }
    if (!kdtree_built || !kdtree) {
        return verticesInRadius;
    }
    pmp::Point query_point(position.x, position.y, position.z);
    float search_radius = radius * 0.05f;
    auto neighbors = kdtree->point_neighbors_in_radius(query_point, search_radius);
    for (const auto& neighbor : neighbors) {
        verticesInRadius.push_back(neighbor.vertex);
    }
    return verticesInRadius;
}
