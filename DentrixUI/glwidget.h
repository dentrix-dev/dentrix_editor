#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QPainter>
#include <glm/glm.hpp>

#include "Brush.h"
#include "Gizmo/gizmo.h"
#include "Gizmo/gizmoComponent.h"
#include "camera.h"
#include "cursorFactory.h"
#include "scene.h"
#include "shader.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent, std::string path, int jaw_index);
    ~GLWidget();

    void loadModel(const std::string &path);  // Function to load new model
    void loadUpperJaw(const std::string& path);
    void loadLowerJaw(const std::string& path);
    void archAlign();
    void rebuildMainScene();
    void setSelectedMeshDirectionalScale(int val, bool xActive, bool yActive, bool zActive);
    void setFreeDeformAddMode(bool isAdd);
    void setDeformationStrength(int value);
    void setBrushSize(int value);
    void updateCursor();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

protected:
    // Main imported scene
    Scene mainScene;
    // Focused scene that contains the selected tooth (and optionally surrounding teeth)
    Scene editScene;
    // Currently rendered scene
    Scene *currentScene;
    bool inMainScene = true;
    void initializeGL() override;          // Runs once when opengl initializes
    void resizeGL(int w, int h) override;  // Handles resizing
    void paintGL() override;               // Rendering logic
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    std::string initialFilePath;
    int jaw_index;    // jaw_index = 0 means lower jaw, jaw_index =1 means upper jaw

    Shader *shader;

    Camera camera;
    int mousePosX;
    int mousePosY;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    std::vector<Mesh *> meshes;
    Mesh *selectedMesh = nullptr;
    bool isRotating = false;

    Gizmo *myGizmo;
    bool isDraggingGizmo = false;
    GizmoComponent *selectedGizmoComponent = nullptr;
    glm::mat4 selectedToothGizmoModelMatrix = glm::mat4(1.0f);

    void saveEditSceneAndReturnToMainScene();
    bool freeDeformAddMode = true;
    Brush brush;
    BrushMode currentBrushMode = BrushMode::Add;
    bool shiftHeld = false;
    bool ctrlHeld = false;
    std::vector<Mesh*> upperJawMeshes;
    std::vector<Mesh*> lowerJawMeshes;
    bool upperJawLoaded = false;
    bool lowerJawLoaded = false;

signals:
    void meshSelectedInMainScene();
    void movedToEditScene();
    void movedToMainScene();

public slots:
    void onEditSceneClicked();
    void setSelectedMeshScale(int scale);
    void setSelectedMeshRotationAngle(int rotationAngleDegrees);
    void updateMeshScale();
    void rotateMesh();
};

#endif  // GLWIDGET_H
