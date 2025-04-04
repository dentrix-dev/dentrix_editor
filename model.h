#pragma once
#include <assimp/scene.h>
#include <vector>
#include "glm/glm.hpp"
#include <string>
#include "shader.h"
#include <QOpenGLFunctions_3_3_Core>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;

    Vertex(glm::vec3 Position, glm::vec3 Normal);
};

class Mesh {
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string name;
    // Needed to reset model back to origin
    glm::vec3 center;
    // Axis Aligned Bounding box
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;

    static bool drawBoundingBox;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string name, glm::vec3 center, glm::vec3 aabb_min, glm::vec3 aabb_max, QOpenGLFunctions_3_3_Core* gl);
    ~Mesh();

    void Draw();

  private:
    unsigned int VAO, VBO, EBO;
    unsigned int VAO_BB, VBO_BB, EBO_BB;
    QOpenGLFunctions_3_3_Core* gl;

    void setup();
    void setupBoundingBox();
};

class Model {
  public:
    glm::vec3 center;
    Model();
    Model(std::string path, QOpenGLFunctions_3_3_Core* gl);
    void Draw(Shader* shader, Mesh* selectedMesh);

    bool Intersect(
        glm::vec3 ray_origin,
        glm::vec3 ray_direction,
        glm::mat4 ModelMatrix,
        Mesh*& intersectedMesh
        );
  private:
    // model data
    std::vector<Mesh> meshes;
    QOpenGLFunctions_3_3_Core* gl;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    bool TestRayOBBIntersection(
        glm::vec3 ray_origin,        // Ray origin, in world space
        glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
        glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
        glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
        glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
        float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
        );
};
