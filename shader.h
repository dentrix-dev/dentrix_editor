#pragma once

#include <string>
#include <QOpenGLFunctions_3_3_Core>

class Shader {
  public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath, QOpenGLFunctions_3_3_Core* gl);
    void use();
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setMatrix4(const std::string &name, float* value);

  private:
    QOpenGLFunctions_3_3_Core* gl;
    void checkCompileErrors(unsigned int shader, std::string type);
};
