#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>

#include "Widgets/loadingtoolbar.h"
#include "Widgets/rightpanelstackedwidget.h"
#include "Widgets/toolbarwidget.h"
#include "glwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    enum ToolMode { None, UniformScale, DirectionalScale, FreeDeformation, Smoothing, Rotate, MoveTooth, MoveJaw };

    static ToolMode toolMode;
    // toDo: refactor into a separate transformationState class
    // static bool inUnformScale;
    // static bool inDirectionalScale;
    // static bool inFreeDeformation;
    // static bool inRotate;
    // static bool inMoveTooth;
    // static bool inMoveJaw;

    void setupCentralUI(const std::string& modelPath, int loadMode);

private:
    Ui::MainWindow* ui;
    GLWidget* glWidget;

    ToolBarWidget* toolbar;
    RightPanelStackedWidget* rightPanelStack;
    void createToolBar();
    LoadingToolBar* loadingToolBar;
    bool upperLoaded = false;
    bool lowerLoaded = false;

private slots:
    void saveModel();
    void onMeshSelectedInMainScene();
    void onMovedToEditScene();
    void onMovedToMainScene();
    void onQActionGroupTriggered(QAction* action);
    void loadUpperJaw();
    void loadLowerJaw();
    void alignJaws();
    void on_actionSave_Model_triggered();
    void on_actionPreferences_triggered();
};
#endif  // MAINWINDOW_H
