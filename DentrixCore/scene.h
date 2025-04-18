#pragma once
#include <assimp/scene.h>
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
      static Mesh* processMesh(aiMesh *mesh, glm::vec3 center, QOpenGLFunctions_3_3_Core* gl);

    bool TestRayOBBIntersection(
        glm::vec3 ray_origin,        // Ray origin, in world space
        glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
        glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
        glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
        glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
        float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
        );

    static bool RayTriangleIntersect(
        const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        float& outIntersectionDistance);
};
