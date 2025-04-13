#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//toDO: refactor later into a utils class
static void printMat4(const glm::mat4& mat) {
    for (int row = 0; row < 4; ++row) {
        std::cout << "[ ";
        for (int col = 0; col < 4; ++col) {
            std::cout << mat[col][row] << " ";
        }
        std::cout << "]\n";
    }
}

Scene::Scene() {

}

Scene::Scene(std::string path, QOpenGLFunctions_3_3_Core* gl) {
    this->gl = gl;
    loadScene(path);
}

Scene::Scene(std::vector<Mesh> meshes, QOpenGLFunctions_3_3_Core* gl) {
    this->gl = gl;
    this->meshes = meshes;
}

void Scene::Draw(Shader* shader, Mesh* selectedMesh) {
    for (int i=0; i<meshes.size(); i++) {
        if (selectedMesh && meshes[i].name == selectedMesh->name) {
            shader->setFloat("color", 0.8f);
        } else {
            shader->setFloat("color", 0.5f);
        }

        // Scaling needs to happen at 0,0
        // Translate the mesh to origin using its center, scale it, translate it back
        glm::mat4 meshfinalTransform = glm::translate(glm::mat4(1.0f), meshes[i].center) * meshes[i].scaleTransform * glm::translate(glm::mat4(1.0f), -1.0f * meshes[i].center);

        // Center the scene
        // In mainScene, this has no effect since the vertices are already centered on first load
        // In editScene, this centers the new mesh array
        meshfinalTransform = glm::translate(meshfinalTransform, -1.0f * center);

        // Send per-mesh model matrix to the shader
        shader->setMatrix4("model", glm::value_ptr(meshfinalTransform));

        meshes[i].Draw();
    }
}

glm::vec3 Scene::getMeshesCenter() {
    float totalX=0.0;
    float totalY=0.0;
    float totalZ=0.0;
    int numMeshes = meshes.size();
    for (int i=0; i < numMeshes; i++) {
        totalX += (meshes[i].aabb_max.x + meshes[i].aabb_min.x) / 2.0;
        totalY += (meshes[i].aabb_max.y + meshes[i].aabb_min.y) / 2.0;
        totalZ += (meshes[i].aabb_max.z + meshes[i].aabb_min.z) / 2.0;
    }
    return glm::vec3(totalX/numMeshes, totalY/numMeshes, totalZ/numMeshes);
}

void Scene::loadScene(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenBoundingBoxes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // Get scene center
    unsigned int numMeshes = scene->mNumMeshes;
    float totalX=0.0;
    float totalY=0.0;
    float totalZ=0.0;
    for (int i=0; i < numMeshes; i++) {
        const aiAABB &aabb = scene->mMeshes[i]->mAABB;
        glm::vec3 aabb_min = glm::vec3(aabb.mMin.x,aabb.mMin.y,aabb.mMin.z);
        glm::vec3 aabb_max = glm::vec3(aabb.mMax.x,aabb.mMax.y,aabb.mMax.z);

        totalX += (aabb_max.x + aabb_min.x) / 2.0;
        totalY += (aabb_max.y + aabb_min.y) / 2.0;
        totalZ += (aabb_max.z + aabb_min.z) / 2.0;
    }
    center = glm::vec3(totalX/numMeshes, totalY/numMeshes, totalZ/numMeshes);

    processNode(scene->mRootNode, scene);
    center = getMeshesCenter();
}

void Scene::processNode(aiNode *node, const aiScene *scene) {
    for (int i=0; i<node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh));
    }

    for (int i=0; i<node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}


Mesh Scene::processMesh(aiMesh *mesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int i=0; i<mesh->mNumVertices; i++) {
        float x = mesh->mVertices[i].x - center.x;
        float y = mesh->mVertices[i].y - center.y;
        float z = mesh->mVertices[i].z - center.z;
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
    glm::vec3 aabb_min = glm::vec3(aabb.mMin.x-center.x,aabb.mMin.y-center.y,aabb.mMin.z-center.z);
    glm::vec3 aabb_max = glm::vec3(aabb.mMax.x-center.x,aabb.mMax.y-center.y,aabb.mMax.z-center.z);

    float centerX = (aabb_max.x + aabb_min.x) / 2.0;
    float centerY = (aabb_max.y + aabb_min.y) / 2.0;
    float centerZ = (aabb_max.z + aabb_min.z) / 2.0;
    glm::vec3 center = glm::vec3(centerX, centerY, centerZ);

    return Mesh(vertices, indices, mesh->mName.C_Str(), center, aabb_min, aabb_max, gl);
}

bool Scene::TestRayOBBIntersection(
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

bool Scene::Intersect(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::mat4 ModelMatrix, Mesh*& intersectedMesh) {
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

void Scene::GetNeighboringMeshNames(std::string meshName, std::string &neighbor1Name, std::string &neighbor2Name)
{
    // Upper jaw teeth ordering in local mapping (not universal mapping)
    std::string upperMeshNames[16] = {"tooth16", "tooth15", "tooth14", "tooth13", "tooth12", "tooth11", "tooth10", "tooth9", "tooth1", "tooth2", "tooth3", "tooth4", "tooth5", "tooth6", "tooth7", "tooth8"};

    int upperMeshIndex;
    for (int i=0; i<16; i++) {
        if (upperMeshNames[i] == meshName)
            upperMeshIndex = i;
    }

    if (upperMeshIndex == 0) {
        neighbor2Name = upperMeshNames[upperMeshIndex+1];
    } else if (upperMeshIndex == 15) {
        neighbor1Name = upperMeshNames[upperMeshIndex-1];
    } else {
        neighbor1Name = upperMeshNames[upperMeshIndex-1];
        neighbor2Name = upperMeshNames[upperMeshIndex+1];
    }
}








