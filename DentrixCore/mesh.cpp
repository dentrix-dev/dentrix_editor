#include "mesh.h"

bool Mesh::drawBoundingBox = true;

Mesh::Mesh(aiMesh *aimesh, glm::vec3 scene_center, QOpenGLFunctions_3_3_Core* gl) {
    this->gl = gl;

    // Create a PMP vertex property for position
    mesh.vertex_property<pmp::Point>("v:point");
    auto vnormals = mesh.vertex_property<pmp::Normal>("v:normal");

    // Map to keep track of added vertices
    std::vector<pmp::Vertex> vertexHandles;
    vertexHandles.reserve(aimesh->mNumVertices);
    vertices.reserve(aimesh->mNumVertices * 3);
    normals.reserve(aimesh->mNumVertices * 3);

    // 1. Add all vertices
    for (unsigned int i = 0; i < aimesh->mNumVertices; ++i) {
        aiVector3D v = aimesh->mVertices[i];
        pmp::Point pos(v.x - scene_center.x, v.y - scene_center.y, v.z - scene_center.z);
        pmp::Vertex vh = mesh.add_vertex(pos);
        vertexHandles.push_back(vh);

        vertices.push_back(pos[0]);
        vertices.push_back(pos[1]);
        vertices.push_back(pos[2]);

        // Handle normals
        pmp::Normal n(0, 0, 0);
        if (aimesh->HasNormals()) {
            aiVector3D aiN = aimesh->mNormals[i];
            n = pmp::Normal(aiN.x, aiN.y, aiN.z);
        }
        vnormals[vh] = n;

        normals.push_back(n[0]);
        normals.push_back(n[1]);
        normals.push_back(n[2]);
    }

    indices.reserve(aimesh->mNumFaces * 3);

    for (unsigned int i = 0; i < aimesh->mNumFaces; ++i) {
        const aiFace& face = aimesh->mFaces[i];
        if (face.mNumIndices == 3) {
            std::vector<pmp::Vertex> faceVerts;
            for (unsigned int j = 0; j < 3; ++j) {
                unsigned int idx = face.mIndices[j];
                faceVerts.push_back(vertexHandles[idx]);
                indices.push_back(idx); // OpenGL index buffer
            }
            mesh.add_face(faceVerts);
        }
    }

    const aiAABB &aabb = aimesh->mAABB;
    aabb_min = glm::vec3(aabb.mMin.x-scene_center.x,aabb.mMin.y-scene_center.y,aabb.mMin.z-scene_center.z);
    aabb_max = glm::vec3(aabb.mMax.x-scene_center.x,aabb.mMax.y-scene_center.y,aabb.mMax.z-scene_center.z);
    center = 0.5f * (aabb_min + aabb_max);
    name = aimesh->mName.C_Str();

    setup();
    setupBoundingBox();
}

Mesh::~Mesh() {

}

void Mesh::setup() {
    gl->glGenVertexArrays(1, &VAO);
    gl->glGenBuffers(1, &VBO_pos);
    gl->glGenBuffers(1, &VBO_norm);
    gl->glGenBuffers(1, &EBO);

    gl->glBindVertexArray(VAO);

    // --- Position VBO (location = 0)
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    gl->glEnableVertexAttribArray(0);

    // --- Normal VBO (location = 1)
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
    gl->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    gl->glEnableVertexAttribArray(1);

    // --- Element Buffer
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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

void Mesh::updateBuffers() {
    // Get properties from the mesh
    auto vpos = mesh.get_vertex_property<pmp::Point>("v:point");
    auto vnormals = mesh.get_vertex_property<pmp::Normal>("v:normal");

    // Update OpenGL buffers
    vertices.clear();
    normals.clear();
    vertices.reserve(mesh.n_vertices() * 3);
    normals.reserve(mesh.n_vertices() * 3);

    // Loop over all vertices in the mesh to update positions and normals
    for (auto v : mesh.vertices()) {
        // Update positions in OpenGL
        const auto& p = vpos[v];
        vertices.push_back(p[0]);
        vertices.push_back(p[1]);
        vertices.push_back(p[2]);

        // Update normals in OpenGL
        const auto& n = vnormals[v];
        normals.push_back(n[0]);
        normals.push_back(n[1]);
        normals.push_back(n[2]);
    }

    // Update position buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    gl->glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    // Update normal buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
    gl->glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(float), normals.data());

    // Unbind buffers
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setScale(float scaleFactor)
{
    currentScale = scaleFactor;
    scaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, 1.0f));
}

void Mesh::setScaleDirectional(float x, float y, float z)
{
    directionalScaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}
