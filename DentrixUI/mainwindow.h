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

private slots:
    void loadModel();
    void onMeshSelectedInMainScene();
    void onMovedToEditScene();
};
#endif // MAINWINDOW_H
