#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <vector>

#include "mesh.h"

class FileHandler
{
public:
    FileHandler();

    static std::vector<Mesh*> readOBJ(std::string& filepath);
    static Mesh* readSTL(std::string& filepath, std::string& name);

    // File type is determined by the file extension
    static void writeMesh(std::string& filepath, pmp::SurfaceMesh& mesh);
};

#endif  // FILEHANDLER_H
