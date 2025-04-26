#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"
#include <QPushButton>
#include <QStackedWidget>
#include <QButtonGroup>
#include "Widgets/rightpanelstackedwidget.h"
#include "Widgets/toolbarwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //toDo: refactor into a separate transformationState class
    static bool inUnformScale;
    static bool inDirectionalScale;
    static bool inFreeDeformation;

private:
    Ui::MainWindow *ui;
    GLWidget *glWidget;

    ToolBarWidget* toolbar;
    RightPanelStackedWidget* rightPanelStack;

    void createToolBar();

private slots:
    void loadModel();
    void onMeshSelectedInMainScene();
    void onMovedToEditScene();
    void onMovedToMainScene();
    void onQActionGroupTriggered(QAction* action);
};
#endif // MAINWINDOW_H
