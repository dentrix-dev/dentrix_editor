#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include "model.h"
#include <QMouseEvent>
#include "camera.h"
#include <glm/glm.hpp>
#include "shader.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent, std::string path);
    ~GLWidget();

    void loadModel(const std::string &path); // Function to load new model
protected:
    Model objectModel;
    void initializeGL() override;  // Runs once when opengl initializes
    void resizeGL(int w, int h) override;  // Handles resizing
    void paintGL() override;  // Rendering logic
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    std::string initialFilePath;

    Shader* shader;

    Camera camera;
    int mousePosX;
    int mousePosY;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    Mesh* selectedMesh = nullptr;
};

#endif // GLWIDGET_H
