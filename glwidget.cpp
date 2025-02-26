#include <iostream>
#include "glwidget.h"
#include "model.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

GLWidget::~GLWidget() {}

const char* vertexShader2 = "#version 330 core\n"
                           "layout (location = 0) in vec3 aPos;\n"

                           "void main() {\n"
                           "gl_Position = vec4(aPos, 1.0);\n"
                           "};\0";

const char* fragmentShader2 = "#version 330 core\n"
                             "out vec4 FragColor;\n"
                             "void main() {\n"
                             "FragColor = vec4(0.5f, 1.0f, 0.5f, 1.0f);\n"
                             "};\0";

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// set background color

    // Shaders
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open("/home/mahmoud/Work/Uni/GP/dentrix_editor/shaders/vertexShader.vs");
        fShaderFile.open("/home/mahmoud/Work/Uni/GP/dentrix_editor/shaders/fragmentShader.fs");
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
    const char *vertexShader = vertexCode.c_str();
    const char *fragmentShader = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShader, NULL);
    glCompileShader(vertex);
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShader, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Load model
    Model sampleModel("/home/mahmoud/Work/Uni/GP/dentrix_editor/models/jaw_upper.obj", this);
    model = sampleModel;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); // adjust viewport to new size
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear screen
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    model.Draw();
}




