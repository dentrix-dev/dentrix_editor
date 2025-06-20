#ifndef MESH_H
#define MESH_H

#include <pmp/surface_mesh.h>

#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Brush.h"

class Mesh : protected QOpenGLFunctions_3_3_Core
{
public:
    pmp::SurfaceMesh surfaceMesh;
    unsigned int tooth_number;
    // Needed to reset model back to origin
    glm::vec3 center;
    // Axis Aligned Bounding box
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
    glm::mat4 scaleTransform = glm::mat4(1.0f);
    glm::mat4 rotateTransform = glm::mat4(1.0f);
    glm::mat4 directionalScaleTransform = glm::mat4(1.0f);

    static bool drawBoundingBox;

    Mesh(pmp::SurfaceMesh& mesh, unsigned int tooth_number);
    ~Mesh();

    void setScale(float scaleFactor);
    void setRotationAngle(float rotationAngle);
    void setScaleDirectional(float x, float y, float z);
    void translate(glm::mat4& translationMatrix);

    void fillHoles();
    void draw();
    void updateVerticesBuffer();
    void updateBuffers();
    void updateBoundingBoxBuffers();
    void updateMeshScale();
    void rotateMesh(glm::vec3 sceneCenter);
    void applyFreeDeformation(Brush& brush, bool isAdd);
    void recalculateBoundingBox();

private:
    unsigned int VAO, VBO, EBO;
    unsigned int VAO_BB, VBO_BB, EBO_BB;
    unsigned int numIndices;
    float currentScale = 1.0f;
    int rotationAngleDegrees = 0;

    void setup();
    void setupBoundingBox();

    void fillHole(pmp::Halfedge h);
};

#endif  // MESH_H
