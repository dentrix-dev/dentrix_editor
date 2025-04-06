#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// -------------- Vertex ---------------
Vertex::Vertex(glm::vec3 Position, glm::vec3 Normal) {
    this->Position = Position;
    this->Normal = Normal;
}

// -------------- Mesh ----------------

bool Mesh::drawBoundingBox = true;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string name, glm::vec3 center, glm::vec3 aabb_min, glm::vec3 aabb_max, QOpenGLFunctions_3_3_Core* gl) {
    this->vertices = vertices;
    this->indices = indices;
    this->name = name;
    this->center = center;
    this->aabb_min = aabb_min;
    this->aabb_max = aabb_max;
    this->gl = gl;

    setup();
    setupBoundingBox();
}

Mesh::~Mesh() {

}

void Mesh::setup() {
    gl->glGenVertexArrays(1, &VAO);
    gl->glGenBuffers(1, &VBO);
    gl->glGenBuffers(1, &EBO);

    gl->glBindVertexArray(VAO);

    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    gl->glBindVertexArray(0);
}

void Mesh::setupBoundingBox() {
    // 1. Calculate the 8 vertices of the bounding box
    std::vector<glm::vec3> bb_vertices;
    bb_vertices.resize(8);

    bb_vertices[0] = glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z); // 0 --- Near bottom left
    bb_vertices[1] = glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z); // 1 --= Near bottom right
    bb_vertices[2] = glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z); // 2 -=- Near top right
    bb_vertices[3] = glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z); // 3 --- Near top left
    bb_vertices[4] = glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z); // 4 --- Far bottom left
    bb_vertices[5] = glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z); // 5 --= Far bottom right
    bb_vertices[6] = glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z); // 6 -=- Far top right
    bb_vertices[7] = glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z); // 7 --- Far top left

    // 2. Define the indices for the 12 lines (edges) of the bounding box
    // Each pair of indices defines one line segment.
    std::vector<unsigned int> bb_indices = {
        // Bottom face
        0, 1, 1, 2, 2, 3, 3, 0,
        // Top face
        4, 5, 5, 6, 6, 7, 7, 4,
        // Connecting edges
        0, 4, 1, 5, 2, 6, 3, 7
    };
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
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, bb_indices.size() * sizeof(unsigned int), bb_indices.data(), GL_STATIC_DRAW);

    // 6. Set up vertex attributes (only position)
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // 7. Unbind VAO (good practice)
    gl->glBindVertexArray(0);

    // Optional: Unbind VBO and EBO after VAO is unbound. VAO remembers these bindings.
    // gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
    // gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw() {
    gl->glBindVertexArray(VAO);
    gl->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
    scaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));
}

// ------------------- Model ----------------
Model::Model() {

}

Model::Model(std::string path, QOpenGLFunctions_3_3_Core* gl) {
    this->gl = gl;
    loadModel(path);
}

void Model::Draw(Shader* shader, Mesh* selectedMesh, glm::mat4 model) {
    for (int i=0; i<meshes.size(); i++) {
        if (selectedMesh && meshes[i].name == selectedMesh->name) {
            shader->setFloat("color", 0.8f);
        } else {
            shader->setFloat("color", 0.5f);
        }

        glm::mat4 meshfinalTransform = meshes[i].scaleTransform * model ;

        // Send per-mesh model matrix to the shader
        shader->setMatrix4("model", glm::value_ptr(meshfinalTransform));

        meshes[i].Draw();
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenBoundingBoxes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);

    float totalX=0.0;
    float totalY=0.0;
    float totalZ=0.0;
    for (int i=0; i<meshes.size(); i++) {
        totalX += meshes[i].center.x;
        totalY += meshes[i].center.y;
        totalZ += meshes[i].center.z;
    }
    center = glm::vec3(totalX / (float)meshes.size(), totalY / (float)meshes.size(), totalZ / (float)meshes.size());
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for (int i=0; i<node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (int i=0; i<node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}


Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int i=0; i<mesh->mNumVertices; i++) {
        float x = mesh->mVertices[i].x;
        float y = mesh->mVertices[i].y;
        float z = mesh->mVertices[i].z;
        glm::vec3 v(x,y,z);
        glm::vec3 n(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertices.push_back(Vertex(v, n));
    }

    for (int i=0; i<mesh->mNumFaces; i++) {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    const aiAABB &aabb = mesh->mAABB;
    glm::vec3 aabb_min = glm::vec3(aabb.mMin.x,aabb.mMin.y,aabb.mMin.z);
    glm::vec3 aabb_max = glm::vec3(aabb.mMax.x,aabb.mMax.y,aabb.mMax.z);

    float centerX = (aabb_max.x + aabb_min.x) / 2.0;
    float centerY = (aabb_max.y + aabb_min.y) / 2.0;
    float centerZ = (aabb_max.z + aabb_min.z) / 2.0;
    glm::vec3 center = glm::vec3(centerX, centerY, centerZ);

    return Mesh(vertices, indices, mesh->mName.C_Str(), center, aabb_min, aabb_max, gl);
}

bool Model::TestRayOBBIntersection(
    glm::vec3 ray_origin,        // Ray origin, in world space
    glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
    glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
    glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
    glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
    float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
    ){

    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if ( fabs(f) > 0.001f ){ // Standard case

            float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1>t2){
                float w=t1;t1=t2;t2=w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if ( t2 < tMax )
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if ( t1 > tMin )
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin )
                return false;

        }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.y)/f;
            float t2 = (e+aabb_max.y)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.z)/f;
            float t2 = (e+aabb_max.z)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                return false;
        }
    }

    intersection_distance = tMin;
    return true;
}

bool Model::Intersect(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::mat4 ModelMatrix, Mesh*& intersectedMesh) {
    std::string meshName = "none";
    bool intersectionFound = false;
    float distance;
    float minDistance = 0.0f;
    for (int i=0; i < meshes.size(); i++) {
        // Skip gingiv mesh
        if (meshes[i].name == "tooth0") {
            continue;
        }
        if (TestRayOBBIntersection(ray_origin, ray_direction, meshes[i].aabb_min, meshes[i].aabb_max, ModelMatrix, distance)) {
            std::cout << meshes[i].name << std::endl;
            if (!intersectionFound) { // First intersection found
                intersectionFound = true;
                minDistance = distance;
                meshName = meshes[i].name;
                intersectedMesh = &(meshes[i]);
            } else { // Check if new intersection is closer
                if (distance < minDistance) {
                    minDistance = distance;
                    meshName = meshes[i].name;
                    intersectedMesh = &(meshes[i]);
                }
            }
        }
    }
    std::cout << "Closest mesh: " << meshName << std::endl;
    return intersectionFound;
}








