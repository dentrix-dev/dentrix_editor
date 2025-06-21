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
#include "Workers/archalignworker.h"
#include "Workers/jawloaderworker.h"
#include "camera.h"
#include "cursorFactory.h"
#include "mcg/dental/segmentation.h"
#include "scene.h"
#include "shader.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent, std::string path, int jaw_index);
    ~GLWidget();

    bool upperJawLoaded = false;
    bool lowerJawLoaded = false;
    std::vector<Mesh *> upperJawMeshes;
    std::vector<Mesh *> lowerJawMeshes;

    void loadModel(const std::string &path);  // Function to load new model
    void loadUpperJaw(const std::string &path);
    void loadUpperJawAsync(const QString &path);
    void applyUpperJawResult(const JawLoadResult &result);
    void applyLowerJawResult(const JawLoadResult &result);
    void loadLowerJaw(const std::string &path);
    void loadLowerJawAsync(const QString &path);
    void archAlign();
    void archAlignAsync();
    void applyArchAlignResult(const ArchAlignData &result);
    void rebuildMainScene();
    void setSelectedMeshDirectionalScale(int val, bool xActive, bool yActive, bool zActive);
    void setFreeDeformAddMode(bool isAdd);
    void setDeformationStrength(int value);
    void setBrushSize(int value);
    void updateCursor();
    void onToolChange();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

protected:
    // Main imported scene
    Scene mainScene;
    // Focused scene that contains the selected tooth (and optionally surrounding teeth)
    Scene editScene;
    // Currently rendered scene
    Scene *currentScene = nullptr;
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
    int jaw_index;  // jaw_index = 0 means lower jaw, jaw_index =1 means upper jaw

    Shader *shader;

    Camera camera;
    int mousePosX;
    int mousePosY;
    bool mouseButtonDown = false;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    std::vector<Mesh *> meshes;
    Mesh *selectedMesh = nullptr;
    bool isUpperJawSelected = false;
    bool isLowerJawSelected = false;

    Gizmo *toothGizmo = nullptr;
    bool isGizmoEnabled = false;
    bool isDraggingGizmo = false;
    GizmoComponent *selectedGizmoComponent = nullptr;
    glm::mat4 gizmoModelMatrix = glm::mat4(1.0f);

    void saveEditSceneAndReturnToMainScene();
    bool isRotating = false;
    bool freeDeformAddMode = true;
    Brush brush;
    BrushMode currentBrushMode = BrushMode::Add;
    bool shiftHeld = false;
    bool ctrlHeld = false;
    std::string upperJawLabelsPath;
    std::string lowerJawLabelsPath;
    pmp::SurfaceMesh upperJawUnsegmented;
    pmp::SurfaceMesh lowerJawUnsegmented;
    std::vector<pmp::Face> upperRemap;
    std::vector<pmp::Face> lowerRemap;
    mcg::Arch upperArch;
    mcg::Arch lowerArch;
    glm::vec3 upperJawCenter = glm::vec3(0.0f);
    glm::vec3 lowerJawCenter = glm::vec3(0.0f);
    bool areArchesAligned = false;

signals:
    void meshSelectedInMainScene();
    void movedToEditScene();
    void movedToMainScene();

public slots:
    void onEditSceneClicked();
    void onResetJawClicked();
    void setSelectedMeshScale(int scale);
    void setSelectedMeshRotationAngle(int rotationAngleDegrees);
    void updateMeshScale();
    void rotateMesh();
};

#endif  // GLWIDGET_H
