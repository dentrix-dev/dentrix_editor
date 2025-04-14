#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "mesh.h"

class MeshManager
{
    std::vector<Mesh*> m_meshes;
    glm::vec3 m_center;

    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh);

public:
    MeshManager(std::string filepath);
    std::vector<Mesh*> GetMeshes();
    glm::vec3 GetCenter();
    glm::vec3 GetMeshesCenter();
};

#endif // MESHMANAGER_H
