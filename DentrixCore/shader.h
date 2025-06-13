#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <string>

class Shader : protected QOpenGLFunctions_3_3_Core
{
public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath);
    void use();
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec3(const std::string &name, float v1, float v2, float v3);
    void setMatrix4(const std::string &name, float *value);

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
