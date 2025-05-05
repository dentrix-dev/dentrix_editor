#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QOpenGLFunctions_3_3_Core>
#include <vector>

#include "mesh.h"

class FileHandler
{
public:
    FileHandler();

    static std::vector<Mesh*> readOBJ(std::string& filepath, QOpenGLFunctions_3_3_Core* gl);
    static Mesh* readSTL(std::string& filepath, std::string& name, QOpenGLFunctions_3_3_Core* gl);
};

#endif  // FILEHANDLER_H
