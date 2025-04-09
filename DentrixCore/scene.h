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
    Scene();
    Scene(std::string path, QOpenGLFunctions_3_3_Core* gl);
    Scene(std::vector<Mesh> meshes, QOpenGLFunctions_3_3_Core* gl);

    void updateCenter();
    void Draw(Shader* shader, Mesh* selectedMesh);
    bool Intersect(
        glm::vec3 ray_origin,
        glm::vec3 ray_direction,
        glm::mat4 SceneMatrix,
        Mesh*& intersectedMesh
        );
  private:
    // model data
    std::vector<Mesh> meshes;
    QOpenGLFunctions_3_3_Core* gl;

    void loadScene(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh);

    bool TestRayOBBIntersection(
        glm::vec3 ray_origin,        // Ray origin, in world space
        glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
        glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
        glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
        glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
        float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
        );
};
