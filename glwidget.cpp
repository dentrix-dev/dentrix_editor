#include <iostream>
#include "glwidget.h"
#include "model.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>


GLWidget::GLWidget(QWidget *parent, std::string path) : QOpenGLWidget(parent), initialFilePath(path) {
}

GLWidget::~GLWidget() {}

void GLWidget::loadModel(const std::string &path)
{
    makeCurrent();
    objectModel = Model(path, this);  // Load the new model
    update();  // Refresh the OpenGL view
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// set background color
    glEnable(GL_DEPTH_TEST);

    // Shaders
    //shader = new Shader("../../shaders/vertexShader.vs", "../../shaders/fragmentShader.fs", this);
    shader = new Shader("../../shaders/vertexShader.vs", "../../shaders/fragmentShader.fs", this);
    shader->use();

    // Setup projection matrix once as it doesn't change
    projection = glm::perspective(
        glm::radians(45.0f), (float)QWidget::width() / (float)QWidget::height(), 0.1f, 1000.0f);
    shader->setMatrix4("projection", glm::value_ptr(projection));

    model = glm::mat4(1.0f);
    shader->setMatrix4("model", glm::value_ptr(model));

    // Load model
    Model sampleModel(initialFilePath, this);
    objectModel = sampleModel;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); // adjust viewport to new size
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen

    view = camera.GetViewMatrix();
    shader->setMatrix4("view", glm::value_ptr(view));

    // Translate model and its meshes to origin
    model = glm::translate(glm::mat4(1.0f), objectModel.center * -1.0f);
    shader->setMatrix4("model", glm::value_ptr(model));
    objectModel.Draw(shader, selectedMesh);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout<<__func__<<std::endl;
    mousePosX = event->position().x();
    mousePosY = event->position().y();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float offsetX = event->position().x() - mousePosX;
    float offsetY = event->position().y() - mousePosY;
    camera.processMouse(offsetX, offsetY);
    mousePosX = event->position().x();
    mousePosY = event->position().y();
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    std::cout<<__func__<<std::endl;
    float mouseX = event->position().x();
    float mouseY = event->position().y();
    glm::vec3 rayOrigin, rayDirection;
    Camera::ScreenPosToWorldRay(mouseX, mouseY, GLWidget::width(), GLWidget::height(), view, projection, rayOrigin, rayDirection);
    std::cout << "Camera front: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << std::endl;
    std::cout << "Direction position: " << rayDirection.x << " " << rayDirection.y << " " << rayDirection.z << std::endl;

    // Ray direction Y is flipped for some reason, flip it back
    rayDirection.y = -1 * rayDirection.y;

    Mesh* intersectedMesh = nullptr;
    bool intersection = objectModel.Intersect(rayOrigin, rayDirection, model, intersectedMesh);
    std::cout << "intersection: " << intersection << std::endl;
    if (intersection && intersectedMesh != nullptr) {
        std::cout << "mesh pointer name: " << intersectedMesh->name << std::endl;
        selectedMesh = intersectedMesh;
    } else {
        std::cout << "nullpointer" << std::endl;
    }
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        camera.addDistance(numPixels.y());
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        camera.addDistance(numSteps.y());
    }

    update();
    event->accept();
}


