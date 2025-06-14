#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <vector>

#include "mesh.h"

class FileHandler
{
public:
    FileHandler();

    static pmp::SurfaceMesh readOBJ(const std::string& filepath, std::vector<pmp::Face>* remap = nullptr);
    static std::vector<Mesh*> readSegmentedOBJ(std::string& filepath);
    static Mesh* readSTL(std::string& filepath, std::string& name);

    // File type is determined by the file extension
    static void writeMesh(std::string& filepath, pmp::SurfaceMesh& mesh);
};

#endif  // FILEHANDLER_H
