#pragma once
#include <vector>
#include "glm/glm.hpp"
#include <string>
#include "shader.h"
#include <QOpenGLFunctions_3_3_Core>
#include "mesh.h"

class Scene {
  public:
    glm::vec3 center = glm::vec3(0.0f);
    std::vector<Mesh*> meshes;
    Scene();
    Scene(std::vector<Mesh*> meshes);

    static std::vector<Mesh*> loadScene(std::string path, QOpenGLFunctions_3_3_Core* gl);
    glm::vec3 getMeshesCenter();
    void Draw(Shader* shader, Mesh* selectedMesh);
    bool Intersect(
        glm::vec3 ray_origin,
        glm::vec3 ray_direction,
        glm::mat4 SceneMatrix,
        Mesh*& intersectedMesh
        );

    bool IntersectTriangles(
        glm::vec3 ray_origin_world,         // Ray origin in world space
        glm::vec3 ray_direction_world,      // Ray direction in world space (normalized)
        glm::mat4 sceneMatrix,              // Base scene transform (applied before mesh transforms)
        Mesh*& outHitMesh,                  // Output: Pointer to the hit mesh
        unsigned int& outHitVertexIndex,    // Output: Index of the closest vertex in the hit mesh
        glm::vec3& outIntersectionPointWorld // Output: Exact intersection point in world space
        );

    static void GetNeighboringMeshNames(std::string meshName, std::string &neighbor1Name, std::string &neighbor2Name);

  private:
    static bool RayTriangleIntersect(
        const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        float& outIntersectionDistance);
};
