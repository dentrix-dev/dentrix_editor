#include "shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// constructor generates the shader on the fly
Shader::Shader(const char *vertexPath, const char *fragmentPath, QOpenGLFunctions_3_3_Core *gl)
{
    this->gl = gl;
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = gl->glCreateShader(GL_VERTEX_SHADER);
    gl->glShaderSource(vertex, 1, &vShaderCode, NULL);
    gl->glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = gl->glCreateShader(GL_FRAGMENT_SHADER);
    gl->glShaderSource(fragment, 1, &fShaderCode, NULL);
    gl->glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = gl->glCreateProgram();
    gl->glAttachShader(ID, vertex);
    gl->glAttachShader(ID, fragment);
    gl->glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer
    // necessary
    gl->glDeleteShader(vertex);
    gl->glDeleteShader(fragment);
}

// activate the shader
// ------------------------------------------------------------------------
void Shader::use()
{
    this->gl->glUseProgram(ID);
}

// utility uniform functions
void Shader::setBool(const std::string &name, bool value)
{
    gl->glUniform1i(gl->glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value)
{
    gl->glUniform1i(gl->glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value)
{
    gl->glUniform1f(gl->glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMatrix4(const std::string &name, float *value)
{
    gl->glUniformMatrix4fv(gl->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            gl->glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog
                      << "\n -- --------------------------------------------------- "
                         "-- "
                      << std::endl;
        }
    } else {
        gl->glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            gl->glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog
                      << "\n -- --------------------------------------------------- "
                         "-- "
                      << std::endl;
        }
    }
}
