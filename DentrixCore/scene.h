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
    glm::vec3 center;
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
};
