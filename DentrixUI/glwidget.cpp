#include <iostream>
#include "glwidget.h"
#include "scene.h"
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
    std::cout << __func__ << std::endl;
    makeCurrent();

    // Delete references to previous meshes
    for (int i=0; i<meshes.size(); i++)
        delete meshes[i];

    meshes = Scene::loadScene(path, this);

    mainScene = Scene(meshes);
    currentScene = &mainScene;
    selectedMesh = nullptr;
    update();  // Refresh the OpenGL view
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// set background color
    glEnable(GL_DEPTH_TEST);

    // Shaders
    shader = new Shader("../../shaders/vertexShader.vs", "../../shaders/fragmentShader.fs", this);
    shader->use();

    // Setup projection matrix once as it doesn't change
    projection = glm::perspective(
        glm::radians(45.0f), (float)QWidget::width() / (float)QWidget::height(), 0.1f, 1000.0f);
    shader->setMatrix4("projection", glm::value_ptr(projection));

    model = glm::mat4(1.0f);
    shader->setMatrix4("model", glm::value_ptr(model));

    // Load model
    meshes = Scene::loadScene(initialFilePath, this);

    mainScene = Scene(meshes);
    currentScene = &mainScene;
    update();
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

    currentScene->Draw(shader, selectedMesh);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout<<__func__<<std::endl;
    mousePosX = event->position().x();
    mousePosY = event->position().y();
    isRotating = false;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float offsetX = event->position().x() - mousePosX;
    float offsetY = event->position().y() - mousePosY;
    if (abs(offsetX) > 3 || abs(offsetY) > 3) {
        isRotating = true;
    }
    camera.processMouse(offsetX, offsetY);
    mousePosX = event->position().x();
    mousePosY = event->position().y();
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(!isRotating){
        std::cout<<__func__<<std::endl;
        float mouseX = event->position().x();
        float mouseY = event->position().y();
        glm::vec3 rayDirection;
        Camera::ScreenPosToWorldRay(mouseX, mouseY, GLWidget::width(), GLWidget::height(), view, projection, rayDirection);
        std::cout << "Camera position: " << camera.position.x << " " << camera.position.y << " " << camera.position.z << std::endl;
        std::cout << "Camera front: " << camera.front.x << " " << camera.front.y << " " << camera.front.z << std::endl;
        std::cout << "Direction position: " << rayDirection.x << " " << rayDirection.y << " " << rayDirection.z << std::endl;

        Mesh* intersectedMesh = nullptr;
        bool intersection = currentScene->Intersect(camera.position, rayDirection, model, intersectedMesh);
        std::cout << "intersection: " << intersection << std::endl;
        if (intersection && intersectedMesh != nullptr) {
            std::cout << "mesh pointer name: " << intersectedMesh->name << std::endl;
            selectedMesh = intersectedMesh;
            if (inMainScene)
                emit meshSelectedInMainScene();
            // selectedMesh->setScale(1.2);
        } else {
            std::cout << "nullpointer" << std::endl;
        }
        update();
    }
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

void GLWidget::saveEditSceneAndReturnToMainScene()
{
    currentScene = &mainScene;
    inMainScene = true;
    update();
}

void GLWidget::setSelectedMeshDirectionalScale(int val, bool xActive, bool yActive, bool zActive)
{
    std::cout<<__func__;
    float scaleF = val/10.0;
    std::cout<<"scale: "<<scaleF<<std::endl;
    if (selectedMesh != nullptr) {
        float x = 1.0f;
        float y = 1.0f;
        float z = 1.0f;
        if (xActive)
            x = val/10.0;
        if (yActive)
            y = val/10.0f;
        if (zActive)
            z = val/10.0f;
        selectedMesh->setScaleDirectional(x, y, z);
        update();
    }
}

void GLWidget::onEditSceneClicked()
{
    if (selectedMesh != nullptr && inMainScene){
        std::string neighbor1Name = "";
        std::string neighbor2Name = "";
        Scene::GetNeighboringMeshNames(selectedMesh->name, neighbor1Name, neighbor2Name);

        std::vector<Mesh*> editMeshes = {selectedMesh};

        // Add neighboring teeth meshes
        if (neighbor1Name != "") {
            for (int i=0; i<meshes.size(); i++) {
                if (meshes[i]->name == neighbor1Name) {
                    editMeshes.push_back(meshes[i]);
                }
            }
        }
        if (neighbor2Name != "") {
            for (int i=0; i<meshes.size(); i++) {
                if (meshes[i]->name == neighbor2Name)
                    editMeshes.push_back(meshes[i]);
            }
        }

        editScene = Scene(editMeshes);
        editScene.center = selectedMesh->center;
        currentScene = &editScene;
        emit movedToEditScene();
        inMainScene = false;
    } else if (!inMainScene) {
        saveEditSceneAndReturnToMainScene();
        emit movedToMainScene();
    }
    update();
}

void GLWidget::setSelectedMeshScale(int scale)
{
    float scaleF = scale/10.0;
    // std::cout<<"scale: "<<scaleF<<std::endl;
    if (selectedMesh != nullptr) {
        selectedMesh->setScale(scaleF);
        update();
    }
}


