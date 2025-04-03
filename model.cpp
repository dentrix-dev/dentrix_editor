#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "model.h"

// -------------- Vertex ---------------
Vertex::Vertex(glm::vec3 Position, glm::vec3 Normal) {
    this->Position = Position;
    this->Normal = Normal;
}

// -------------- Mesh ----------------
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 center, QOpenGLFunctions_3_3_Core* gl) {
    this->vertices = vertices;
    this->indices = indices;
    this->center = center;
    this->gl = gl;

    setup();
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

void Mesh::Draw() {
    gl->glBindVertexArray(VAO);
    gl->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);
}

// ------------------- Model ----------------
Model::Model() {

}

Model::Model(std::string path, QOpenGLFunctions_3_3_Core* gl) {
    this->gl = gl;
    loadModel(path);
}

void Model::Draw() {
    for (int i=0; i<meshes.size(); i++) {
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
    float centerX = (aabb.mMax.x + aabb.mMin.x) / 2.0;
    float centerY = (aabb.mMax.y + aabb.mMin.y) / 2.0;
    float centerZ = (aabb.mMax.z + aabb.mMin.z) / 2.0;
    glm::vec3 center = glm::vec3(centerX, centerY, centerZ);

    return Mesh(vertices, indices, center, gl);
}










