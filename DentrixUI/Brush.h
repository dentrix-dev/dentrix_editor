#pragma once

#include <glm/glm.hpp>
#include "pmp/surface_mesh.h"
#include "mcg/mesh_kdtree.h"
#include <memory>

class Brush {
public:
    Brush();

    void setRadius(float r);
    float getRadius();
    void setStrength(int s);
    int getStrength();

    void setPosition(glm::vec3& pos);
    glm::vec3 getPosition();

    std::vector<pmp::Vertex> getVerticesInRadius(const pmp::SurfaceMesh& mesh);

private:
    float radius;
    int strength;
    glm::vec3 position;
    std::unique_ptr<mcg::Mesh_KDTree> kdtree;
    bool kdtree_built = false;
    const pmp::SurfaceMesh* current_mesh_ptr = nullptr;
};
