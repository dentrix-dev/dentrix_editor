#pragma once
#include <assimp/scene.h>
#include <vector>
#include "glm/glm.hpp"
#include <string>
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
    // Needed to reset model back to origin
    glm::vec3 center;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 center, QOpenGLFunctions_3_3_Core* gl);
    ~Mesh();

    void Draw();

  private:
    unsigned int VAO, VBO, EBO;
    QOpenGLFunctions_3_3_Core* gl;

    void setup();
};

class Model {
  public:
    Model();
    Model(std::string path, QOpenGLFunctions_3_3_Core* gl);
    void Draw();

  private:
    // model data
    std::vector<Mesh> meshes;
    QOpenGLFunctions_3_3_Core* gl;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};
