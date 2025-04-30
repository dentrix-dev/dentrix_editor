#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include <pmp/surface_mesh.h>

class Mesh {
  public:
    pmp::SurfaceMesh mesh;
    std::string name;
    // Needed to reset model back to origin
    glm::vec3 center;
    // Axis Aligned Bounding box
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
    glm::mat4 scaleTransform = glm::mat4(1.0f);
    glm::mat4 directionalScaleTransform = glm::mat4(1.0f);

    static bool drawBoundingBox;

    Mesh(pmp::SurfaceMesh& mesh, std::string name, QOpenGLFunctions_3_3_Core* gl);
    ~Mesh();

    void setScale(float scaleFactor);
    void setScaleDirectional(float x, float y, float z);

    bool testRayOBBIntersection(
        glm::vec3 ray_origin,        // Ray origin, in world space
        glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
        glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
        float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
    );
    void draw();
    void updateVerticesBuffer();
    void updateBuffers();
    void updateBoundingBoxBuffers();
    void updateMeshScale(glm::vec3 sceneCenter);

  private:
    unsigned int VAO, VBO_pos, VBO_norm, EBO;
    unsigned int VAO_BB, VBO_BB, EBO_BB;
    unsigned int numIndices;
    QOpenGLFunctions_3_3_Core* gl;
    float currentScale = 1.0f;

    void setup();
    void setupBoundingBox();
};

#endif // MESH_H
