#include "filehandler.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "pmp/io/io.h"
#include "pmp/surface_mesh.h"

FileHandler::FileHandler() {}

pmp::SurfaceMesh FileHandler::readOBJ(const std::string& filepath, std::vector<pmp::Face>* remap)
{
    pmp::SurfaceMesh mesh;
    pmp::read(mesh, filepath, remap);
    return mesh;
}

// This function expects a correctly formatted OBJ file
// o tooth0
// v .. .. ..
// f .. .. ..
std::vector<Mesh*> FileHandler::readSegmentedOBJ(std::string& filepath)
{
    std::ifstream file(filepath);
    std::vector<Mesh*> meshes;

    if (!file) {
        std::cerr << "Could not open file: " << filepath << std::endl;
        return meshes;
    }

    unsigned int vertexIndex = 0;
    std::string name = "";
    std::vector<float> vertices = {};
    std::vector<unsigned int> indices = {};
    std::vector<pmp::Vertex> vhandles = {};

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "o") {
            if (!name.empty()) {
                vertexIndex += vertices.size() / 3;

                pmp::SurfaceMesh surfaceMesh;
                for (int i = 0; i < vertices.size(); i = i + 3) {
                    vhandles.push_back(
                        surfaceMesh.add_vertex(pmp::Point(vertices[i], vertices[i + 1], vertices[i + 2])));
                }
                for (int i = 0; i < indices.size(); i = i + 3) {
                    std::vector<pmp::Vertex> faceVertices = {vhandles[indices[i]], vhandles[indices[i + 1]],
                                                             vhandles[indices[i + 2]]};
                    surfaceMesh.add_face(faceVertices);
                }

                if (name == "tooth0") surfaceMesh.add_vertex_property<bool>("v:selected");

                meshes.push_back(new Mesh(surfaceMesh, 0));
                vhandles.clear();
                vertices.clear();
                indices.clear();
            }
            iss >> name;
        } else if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if (prefix == "f") {
            unsigned int x, y, z;
            iss >> x >> y >> z;
            indices.push_back(x - 1 - vertexIndex);
            indices.push_back(y - 1 - vertexIndex);
            indices.push_back(z - 1 - vertexIndex);
        }
    }

    // Push the last mesh
    if (!name.empty()) {
        pmp::SurfaceMesh surfaceMesh;
        for (int i = 0; i < vertices.size(); i = i + 3) {
            vhandles.push_back(surfaceMesh.add_vertex(pmp::Point(vertices[i], vertices[i + 1], vertices[i + 2])));
        }
        for (int i = 0; i < indices.size(); i = i + 3) {
            std::vector<pmp::Vertex> faceVertices = {vhandles[indices[i]], vhandles[indices[i + 1]],
                                                     vhandles[indices[i + 2]]};
            surfaceMesh.add_face(faceVertices);
        }

        meshes.push_back(new Mesh(surfaceMesh, 0));
        vertices.clear();
        indices.clear();
    }

    file.close();  // Close the file when done
    return meshes;
}

Mesh* FileHandler::readSTL(std::string& filepath, std::string& name)
{
    pmp::SurfaceMesh surfaceMesh;
    pmp::read(surfaceMesh, filepath);

    return new Mesh(surfaceMesh, 0);
}

void FileHandler::writeMesh(std::string& filepath, pmp::SurfaceMesh& mesh)
{
    pmp::write(mesh, filepath);
}
