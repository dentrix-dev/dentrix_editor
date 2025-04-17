#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include "vertex.h"

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
    glm::mat4 scaleTransform = glm::mat4(1.0f);
    glm::mat4 directionalScaleTransform = glm::mat4(1.0f);

    static bool drawBoundingBox;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string name, glm::vec3 center, glm::vec3 aabb_min, glm::vec3 aabb_max, QOpenGLFunctions_3_3_Core* gl);
    ~Mesh();

    void Draw();

    void setScale(float scaleFactor);
    void setScaleDirectional(float x, float y, float z);

  private:
    unsigned int VAO, VBO, EBO;
    unsigned int VAO_BB, VBO_BB, EBO_BB;
    QOpenGLFunctions_3_3_Core* gl;
    float currentScale = 1.0f;

    void setup();
    void setupBoundingBox();
};

#endif // MESH_H
