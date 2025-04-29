#include "filehandler.h"
#include <iostream>
#include <fstream>

FileHandler::FileHandler() {}

// This function expects a correctly formatted OBJ file
// o tooth0
// v .. .. ..
// f .. .. ..
std::vector<Mesh*> FileHandler::readOBJ(std::string &filepath, QOpenGLFunctions_3_3_Core* gl) {
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
                std::cout << "Saving mesh: " << name << std::endl;
                std::cout << "vertices: " << vertices[0] << vertices[1] << vertices[2] << std::endl;
                std::cout << vertices.size() << std::endl;
                std::cout << indices.size() << std::endl;
                vertexIndex += vertices.size() / 3;

                pmp::SurfaceMesh surfaceMesh;
                for (int i=0; i<vertices.size(); i=i+3) {
                    vhandles.push_back(surfaceMesh.add_vertex(pmp::Point(vertices[i],vertices[i+1],vertices[i+2])));
                }
                std::cout << "vertices added" << std::endl;
                for (int i=0; i<indices.size(); i=i+3) {
                    std::vector<pmp::Vertex> faceVertices = {vhandles[indices[i]],vhandles[indices[i+1]],vhandles[indices[i+2]]};
                    if (i == 0) {
                        std::cout << "fv: " << faceVertices[0] << " " << faceVertices[1] << " " << faceVertices[2] << std::endl;
                    }
                    if (faceVertices.size() != 3)
                        std::cout << "size: " << faceVertices.size() << std::endl;
                    surfaceMesh.add_face(faceVertices);
                }
                std::cout << "faces added" << std::endl;

                meshes.push_back(new Mesh(surfaceMesh, name, gl));
                vhandles.clear();
                vertices.clear();
                indices.clear();
            }
            iss >> name;
        } else if (prefix == "v") {
            float x,y,z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if (prefix == "f") {
            unsigned int x,y,z;
            iss >> x >> y >> z;
            indices.push_back(x - 1 - vertexIndex);
            indices.push_back(y - 1 - vertexIndex);
            indices.push_back(z - 1 - vertexIndex);
        }
    }

    // Push the last mesh
    if (!name.empty()) {
        pmp::SurfaceMesh surfaceMesh;
        for (int i=0; i<vertices.size(); i=i+3) {
            vhandles.push_back(surfaceMesh.add_vertex(pmp::Point(vertices[i],vertices[i+1],vertices[i+2])));
        }
        for (int i=0; i<indices.size(); i=i+3) {
            std::vector<pmp::Vertex> faceVertices = {vhandles[indices[i]],vhandles[indices[i+1]],vhandles[indices[i+2]]};
            surfaceMesh.add_face(faceVertices);
        }

        meshes.push_back(new Mesh(surfaceMesh, name, gl));
        vertices.clear();
        indices.clear();
    }

    file.close(); // Close the file when done
    return meshes;
}
