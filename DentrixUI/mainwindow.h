#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"
#include <QPushButton>


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

private:
    Ui::MainWindow *ui;
    GLWidget *glWidget;
    QPushButton *editButton;

    void createToolBar();

    const QString editButtonInactiveStyleSheet = "background-color: gray; color: white; padding: 5px;";
    const QString editButtonActiveStyleSheet = "background-color: #3498db; color: white; padding: 5px;";

private slots:
    void loadModel();
    void onMeshSelectedInMainScene();
    void onMovedToEditScene();
    void onMovedToMainScene();
};
#endif // MAINWINDOW_H
