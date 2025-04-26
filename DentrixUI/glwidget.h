#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include "scene.h"
#include <QMouseEvent>
#include "camera.h"
#include <glm/glm.hpp>
#include "shader.h"
#include <QPainter>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent, std::string path);
    ~GLWidget();

    void loadModel(const std::string &path); // Function to load new model
    void setSelectedMeshDirectionalScale(int val, bool xActive, bool yActive, bool zActive);
    void setFreeDeformAddMode(bool isAdd);
    void setDeformationStrength(int value);
    void setBrushSize(int value);
    void updateCursor();

protected:
    // Main imported scene
    Scene mainScene;
    // Focused scene that contains the selected tooth (and optionally surrounding teeth)
    Scene editScene;
    // Currently rendered scene
    Scene* currentScene;
    bool inMainScene = true;
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

    std::vector<Mesh*> meshes;
    Mesh* selectedMesh = nullptr;
    bool isRotating = false;

    void saveEditSceneAndReturnToMainScene();
    bool freeDeformAddMode = true;
    QCursor addCursor;
    QCursor removeCursor;
    QCursor defaultCursor;
    void initializeCursors();
    QCursor createAddCursor(int size);
    QCursor createRemoveCursor(int size);
    int brushSize = 15;

signals:
    void meshSelectedInMainScene();
    void movedToEditScene();
    void movedToMainScene();

public slots:
    void onEditSceneClicked();
    void setSelectedMeshScale(int scale);
    void updateMeshScale();

};

#endif // GLWIDGET_H
