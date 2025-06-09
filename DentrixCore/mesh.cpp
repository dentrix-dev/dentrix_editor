#include "mesh.h"

#include <pmp/algorithms/differential_geometry.h>
#include <pmp/algorithms/normals.h>
#include <pmp/algorithms/utilities.h>
#include <pmp/bounding_box.h>
#include <pmp/surface_mesh.h>
#include <utils.h>

#include <vector>

#include "pmp/algorithms/fairing.h"

bool Mesh::drawBoundingBox = true;

Mesh::Mesh(pmp::SurfaceMesh& input_mesh, std::string name, QOpenGLFunctions_3_3_Core* gl)
{
    this->gl = gl;
    this->name = name;
    surfaceMesh = input_mesh;

    // Calculate per-face normals for vertices
    // (vertex has a normal for each face it's a part of)
    pmp::face_normals(surfaceMesh);

    // Calculate mesh bounding box and center
    pmp::BoundingBox aabb = pmp::bounds(surfaceMesh);
    aabb_min = Utils::pmpPointToGlm(aabb.min());
    aabb_max = Utils::pmpPointToGlm(aabb.max());
    center = 0.5f * (aabb_min + aabb_max);

    // Update bounding box
    // aabb_min -= center;
    // aabb_max -= center;

    setup();
    setupBoundingBox();
}

Mesh::~Mesh() {}

void Mesh::setup()
{
    auto vpos = surfaceMesh.vertex_property<pmp::Point>("v:point");
    auto fnormal = surfaceMesh.face_property<pmp::Normal>("f:normal");

    // Stores position and normals
    std::vector<float> vertices = {};
    std::vector<unsigned int> indices = {};

    unsigned int index = 0;
    for (auto f : surfaceMesh.faces()) {
        pmp::Normal n = fnormal[f];
        std::vector<unsigned int> faceIndices;

        for (auto v : surfaceMesh.vertices(f)) {
            pmp::Point p = vpos[v];

            vertices.push_back(p[0]);
            vertices.push_back(p[1]);
            vertices.push_back(p[2]);

            vertices.push_back(n[0]);
            vertices.push_back(n[1]);
            vertices.push_back(n[2]);

            faceIndices.push_back(index++);
        }

        if (faceIndices.size() == 3) {
            indices.push_back(faceIndices[0]);
            indices.push_back(faceIndices[1]);
            indices.push_back(faceIndices[2]);
        } else {
            qWarning() << "Non-triangular face detected!";
        }
    }
    numIndices = indices.size();

    gl->glGenVertexArrays(1, &VAO);
    gl->glGenBuffers(1, &VBO);
    gl->glGenBuffers(1, &EBO);

    gl->glBindVertexArray(VAO);

    // Vertices VBO
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    gl->glEnableVertexAttribArray(0);

    // Normal attribute
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    gl->glEnableVertexAttribArray(1);

    // --- Element Buffer
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    gl->glBindVertexArray(0);
}

void Mesh::setupBoundingBox()
{
    // 1. Calculate the 8 vertices of the bounding box
    std::vector<glm::vec3> bb_vertices;
    bb_vertices.resize(8);

    bb_vertices[0] = glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z);  // 0 --- Near bottom left
    bb_vertices[1] = glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z);  // 1 --= Near bottom right
    bb_vertices[2] = glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z);  // 2 -=- Near top right
    bb_vertices[3] = glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z);  // 3 --- Near top left
    bb_vertices[4] = glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z);  // 4 --- Far bottom left
    bb_vertices[5] = glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z);  // 5 --= Far bottom right
    bb_vertices[6] = glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z);  // 6 -=- Far top right
    bb_vertices[7] = glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z);  // 7 --- Far top left

    // 2. Define the indices for the 12 lines (edges) of the bounding box
    // Each pair of indices defines one line segment.
    std::vector<unsigned int> bb_indices = {// Bottom face
                                            0, 1, 1, 2, 2, 3, 3, 0,
                                            // Top face
                                            4, 5, 5, 6, 6, 7, 7, 4,
                                            // Connecting edges
                                            0, 4, 1, 5, 2, 6, 3, 7};
    // Total 24 indices for 12 lines

    // 3. Generate and bind OpenGL objects
    gl->glGenVertexArrays(1, &VAO_BB);
    gl->glGenBuffers(1, &VBO_BB);
    gl->glGenBuffers(1, &EBO_BB);

    gl->glBindVertexArray(VAO_BB);

    // 4. Buffer vertex data (only positions)
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_BB);
    // Use bb_vertices data and size. Note the size calculation uses sizeof(glm::vec3)
    gl->glBufferData(GL_ARRAY_BUFFER, bb_vertices.size() * sizeof(glm::vec3), bb_vertices.data(), GL_STATIC_DRAW);

    // 5. Buffer index data
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BB);
    // Use bb_indices data and size
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, bb_indices.size() * sizeof(unsigned int), bb_indices.data(),
                     GL_STATIC_DRAW);

    // 6. Set up vertex attributes (only position)
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // 7. Unbind VAO (good practice)
    gl->glBindVertexArray(0);

    // Optional: Unbind VBO and EBO after VAO is unbound. VAO remembers these bindings.
    // gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
    // gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Fills a hole in the mesh, defined by a halfedge that lies on the hole boundary
// Creates vertex in the middle of the hole, connects it to hole boundary vertices and smooths them
void Mesh::fillHole(pmp::Halfedge h)
{
    pmp::Point centerPoint;
    std::vector<pmp::Vertex> holeVertices;

    assert(surfaceMesh.is_boundary(h));  // Make sure it's a boundary halfedge

    // Find the vertices on the hole boundary
    pmp::Halfedge start = h;
    pmp::Halfedge he = start;
    do {
        pmp::Vertex v = surfaceMesh.from_vertex(he);
        holeVertices.push_back(v);
        he = surfaceMesh.next_halfedge(he);
    } while (he != start);

    // Find the center of the hole
    for (pmp::Vertex v : holeVertices) {
        centerPoint += surfaceMesh.position(v);
    }
    centerPoint /= (float)holeVertices.size();

    // Create new vertex and connect it to hole boundary vertices
    pmp::Vertex centerVertex = surfaceMesh.add_vertex(centerPoint);
    for (int i = 0; i < holeVertices.size() - 1; i++) {
        std::vector<pmp::Vertex> face = {centerVertex, holeVertices[i], holeVertices[i + 1]};
        surfaceMesh.add_face(face);
    }
    std::vector<pmp::Vertex> face = {holeVertices[0], centerVertex, holeVertices[holeVertices.size() - 1]};
    surfaceMesh.add_face(face);

    // Create a subMesh with only hole vertices and neighbors to speed up smoothing
    pmp::SurfaceMesh subMesh = surfaceMesh;
    auto vselected = subMesh.get_vertex_property<bool>("v:selected");
    auto vkeep = subMesh.add_vertex_property<bool>("v:keep");
    // Mapping to keep track of changed vertices, as deleting vertices changes their indices
    auto voriginal = subMesh.add_vertex_property<unsigned int>("v:original");

    // Mark hole vertices as selected
    // Mark hole vertices and their neighbors as "keep"
    for (unsigned int i = 0; i < holeVertices.size(); i++) {
        pmp::Vertex v = holeVertices[i];
        vkeep[v] = true;
        vselected[v] = true;
        voriginal[v] = i + 1;
        for (pmp::Vertex vv : surfaceMesh.vertices(v)) {
            vkeep[vv] = true;
        }
    }

    // Delete all unkept vertices along with their faces
    for (pmp::Vertex v : subMesh.vertices()) {
        if (!vkeep[v]) subMesh.delete_vertex(v);
    }
    subMesh.garbage_collection();

    // Smooth the filled hole, defined by vselected
    pmp::fair(subMesh, 1);

    // Copy the changed vertices to the original mesh
    for (pmp::Vertex v : subMesh.vertices()) {
        if (voriginal[v] != 0) {
            surfaceMesh.position(holeVertices[voriginal[v] - 1]) = subMesh.position(v);
        }
    }

    pmp::face_normals(surfaceMesh);
    updateBuffers();
}

void Mesh::updateBoundingBoxBuffers()
{
    std::vector<glm::vec3> bb_vertices;
    bb_vertices.resize(8);

    bb_vertices[0] = glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z);  // 0 --- Near bottom left
    bb_vertices[1] = glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z);  // 1 --= Near bottom right
    bb_vertices[2] = glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z);  // 2 -=- Near top right
    bb_vertices[3] = glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z);  // 3 --- Near top left
    bb_vertices[4] = glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z);  // 4 --- Far bottom left
    bb_vertices[5] = glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z);  // 5 --= Far bottom right
    bb_vertices[6] = glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z);  // 6 -=- Far top right
    bb_vertices[7] = glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z);  // 7 --- Far top left

    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_BB);
    // Use bb_vertices data and size. Note the size calculation uses sizeof(glm::vec3)
    gl->glBufferData(GL_ARRAY_BUFFER, bb_vertices.size() * sizeof(glm::vec3), bb_vertices.data(), GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateVerticesBuffer()
{
    auto vpos = surfaceMesh.vertex_property<pmp::Point>("v:point");
    auto fnormal = surfaceMesh.face_property<pmp::Normal>("f:normal");

    std::vector<float> vertices;
    vertices.reserve(surfaceMesh.n_faces() * 3 * 3 * 2);

    for (auto f : surfaceMesh.faces()) {
        pmp::Normal n = fnormal[f];
        for (auto v : surfaceMesh.vertices(f)) {
            pmp::Point p = vpos[v];
            vertices.push_back(p[0]);
            vertices.push_back(p[1]);
            vertices.push_back(p[2]);
            vertices.push_back(n[0]);
            vertices.push_back(n[1]);
            vertices.push_back(n[2]);
        }
    }
    // Update position buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), nullptr,
                     GL_DYNAMIC_DRAW);  // orphan old buffer
    gl->glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateBuffers()
{
    auto vpos = surfaceMesh.vertex_property<pmp::Point>("v:point");
    auto fnormal = surfaceMesh.face_property<pmp::Normal>("f:normal");

    std::vector<float> vertices = {};
    std::vector<unsigned int> indices = {};

    unsigned int index = 0;
    for (auto f : surfaceMesh.faces()) {
        pmp::Normal n = fnormal[f];
        std::vector<unsigned int> faceIndices;

        for (auto v : surfaceMesh.vertices(f)) {
            pmp::Point p = vpos[v];

            vertices.push_back(p[0]);
            vertices.push_back(p[1]);
            vertices.push_back(p[2]);

            vertices.push_back(n[0]);
            vertices.push_back(n[1]);
            vertices.push_back(n[2]);

            faceIndices.push_back(index++);
        }

        if (faceIndices.size() == 3) {
            indices.push_back(faceIndices[0]);
            indices.push_back(faceIndices[1]);
            indices.push_back(faceIndices[2]);
        } else {
            qWarning() << "Non-triangular face detected!";
        }
    }
    numIndices = indices.size();

    // Update vertices buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Update indices buffer
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    // Unbind buffers
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw()
{
    gl->glBindVertexArray(VAO);
    gl->glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);

    if (drawBoundingBox) {
        gl->glBindVertexArray(VAO_BB);
        // Draw 12 lines using the 24 indices provided
        gl->glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        gl->glBindVertexArray(0);
    }
}

void Mesh::setScale(float scaleFactor)
{
    currentScale = scaleFactor;
    scaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, 1.0f));
}

void Mesh::setRotationAngle(float rotationAngle)
{
    this->rotationAngleDegrees = rotationAngle;
    glm::vec3 zAxis(0.0f, 0.0f, 1.0f);
    rotateTransform = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), zAxis);
}

void Mesh::setScaleDirectional(float x, float y, float z)
{
    directionalScaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Mesh::updateMeshScale(glm::vec3 sceneCenter)
{
    pmp::Point center = pmp::centroid(surfaceMesh);
    // pmp::Point center = Utils::glmToPmpPoint(sceneCenter);
    // Scale vertices around center
    for (auto v : surfaceMesh.vertices()) {
        pmp::Point p = surfaceMesh.position(v);
        p = center + (p - center) * currentScale;
        surfaceMesh.position(v) = p;
    }
    pmp::face_normals(surfaceMesh);
    pmp::vertex_normals(surfaceMesh);
    scaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    updateBuffers();
}

void Mesh::rotateMesh(glm::vec3 sceneCenter)
{
    // Get the center of the mesh (you could also use Utils::glmToPmpPoint(sceneCenter))
    pmp::Point center = Utils::glmToPmpPoint(this->center);
    // Define vertical axis (Y-axis)
    glm::vec3 zAxis(0.0f, 0.0f, 1.0f);

    // Create a rotation matrix
    float angleRadians = glm::radians((float)rotationAngleDegrees);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleRadians, zAxis);

    // Apply rotation to each vertex
    for (auto v : surfaceMesh.vertices()) {
        pmp::Point p = surfaceMesh.position(v);
        glm::vec3 pos = Utils::pmpPointToGlm(p);  // Convert to glm

        // Translate to origin, rotate, translate back
        pos -= glm::vec3(center[0], center[1], center[2]);
        pos = glm::vec3(rotation * glm::vec4(pos, 1.0f));
        pos += glm::vec3(center[0], center[1], center[2]);

        surfaceMesh.position(v) = Utils::glmToPmpPoint(pos);
    }

    // Recalculate normals
    pmp::face_normals(surfaceMesh);
    pmp::vertex_normals(surfaceMesh);

    // Update model transformation matrix (optional, for rendering)
    rotateTransform = glm::mat4(1.0);

    // Update GPU buffers
    updateBuffers();
}

void Mesh::applyFreeDeformation(Brush& brush, bool isAdd)
{
    auto vpos = surfaceMesh.vertex_property<pmp::Point>("v:point");
    if (!surfaceMesh.has_vertex_property("v:normal")) {
        pmp::vertex_normals(surfaceMesh);
    }
    auto vnormal = surfaceMesh.vertex_property<pmp::Normal>("v:normal");
    std::vector<pmp::Vertex> vertices = brush.getVerticesInRadius(surfaceMesh);
    const float strengthMultiplier = 0.0001f;
    float adjustedStrength = brush.getStrength() * strengthMultiplier;
    for (auto v : vertices) {
        pmp::Point p = vpos[v];
        glm::vec3 vertexPos(p[0], p[1], p[2]);

        float distance = glm::distance(vertexPos, brush.getPosition());

        float falloff = 1.0f - (distance / brush.getRadius());
        falloff = pow(falloff, 3.0f);

        pmp::Normal n = vnormal[v];
        glm::vec3 normal(n[0], n[1], n[2]);
        normal = glm::normalize(normal);

        float direction = isAdd ? 1.0f : -1.0f;
        float strength = adjustedStrength * falloff;

        glm::vec3 displacement = normal * strength * direction;
        pmp::Point newPos(p[0] + displacement.x, p[1] + displacement.y, p[2] + displacement.z);
        vpos[v] = newPos;
    }
    pmp::vertex_normals(surfaceMesh);
    pmp::face_normals(surfaceMesh);
    updateBuffers();
}
