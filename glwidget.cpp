#include <iostream>
#include "glwidget.h"
#include "model.h"
#include "shader.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

GLWidget::~GLWidget() {}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// set background color

    // Shaders
    Shader shader("../../shaders/vertexShader.vs", "../../shaders/fragmentShader.fs", this);
    shader.use();
    // Load model
    Model sampleModel("../../models/jaw_upper.obj", this);
    model = sampleModel;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); // adjust viewport to new size
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear screen
    model.Draw();
}




