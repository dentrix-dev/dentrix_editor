#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "meshmanager.h"

MeshManager::MeshManager(std::string filepath)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenBoundingBoxes);

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
    m_center = glm::vec3(totalX/numMeshes, totalY/numMeshes, totalZ/numMeshes);

    processNode(scene->mRootNode, scene);
    m_center = getMeshesCenter();
}

std::vector<Mesh *> MeshManager::GetMeshes()
{
    return m_meshes;
}

glm::vec3 MeshManager::GetCenter()
{
    return m_center;
}

glm::vec3 MeshManager::GetMeshesCenter()
{
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


void MeshManager::processNode(aiNode *node, const aiScene *scene) {
    for (int i=0; i<node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh));
    }

    for (int i=0; i<node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}


Mesh MeshManager::processMesh(aiMesh *mesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int i=0; i<mesh->mNumVertices; i++) {
        float x = mesh->mVertices[i].x - m_center.x;
        float y = mesh->mVertices[i].y - m_center.y;
        float z = mesh->mVertices[i].z - m_center.z;
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
    glm::vec3 aabb_min = glm::vec3(aabb.mMin.x-m_center.x,aabb.mMin.y-m_center.y,aabb.mMin.z-m_center.z);
    glm::vec3 aabb_max = glm::vec3(aabb.mMax.x-m_center.x,aabb.mMax.y-m_center.y,aabb.mMax.z-m_center.z);

    float centerX = (aabb_max.x + aabb_min.x) / 2.0;
    float centerY = (aabb_max.y + aabb_min.y) / 2.0;
    float centerZ = (aabb_max.z + aabb_min.z) / 2.0;
    glm::vec3 center = glm::vec3(centerX, centerY, centerZ);

    return Mesh(vertices, indices, mesh->mName.C_Str(), center, aabb_min, aabb_max, gl);
}
