#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QToolBar>
#include <QWidgetAction>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , glWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Dentrix Editor");
    resize(QApplication::primaryScreen()->geometry().width(), QApplication::primaryScreen()->geometry().height());
    connect(ui->actionLoad_Model, &QAction::triggered, this, &MainWindow::loadModel);
    createToolBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createToolBar()
{
    // Create a toolbar
    QToolBar *toolBar = new QToolBar("Side Toolbar", this);

    // Create actions (buttons) to add to the toolbar
    QAction *uniformTranslateAction = new QAction("Uniform Scale", this);
    QAction *saveAction = new QAction("Directional Scale", this);
    QAction *exitAction = new QAction("Free Deformation", this);
    // Add a spacer widget to push following widgets to the bottom
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidgetAction *spacerAction = new QWidgetAction(this);
    spacerAction->setDefaultWidget(spacer);
    // Create a styled blue "Edit" button
    editButton = new QPushButton("Edit");
    editButton->setStyleSheet(editButtonInactiveStyleSheet);
    // Wrap it in a QWidgetAction so it can go in the QToolBar
    QWidgetAction *editAction = new QWidgetAction(this);
    editAction->setDefaultWidget(editButton);

    // Add the actions to the toolbar
    toolBar->addAction(uniformTranslateAction);
    toolBar->addAction(saveAction);
    toolBar->addSeparator();
    toolBar->addAction(exitAction);
    toolBar->addAction(spacerAction);
    toolBar->addAction(editAction);

    // Add toolbar to the left side
    addToolBar(Qt::LeftToolBarArea, toolBar);
}

void MainWindow::loadModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "../../models", "Model Files (*.obj *.stl *.ply)");
    if (!filePath.isEmpty()) {
        if (!glWidget){
            glWidget = new GLWidget(this, filePath.toStdString());
            connect(glWidget, &GLWidget::meshSelectedInMainScene, this, &MainWindow::onMeshSelectedInMainScene);
            connect(glWidget, &GLWidget::movedToEditScene, this, &MainWindow::onMovedToEditScene);
            connect(editButton, &QPushButton::clicked, glWidget, &GLWidget::onEditSceneClicked);
            setCentralWidget(glWidget);
        } else {
            glWidget->loadModel(filePath.toStdString());
        }
    }
}

void MainWindow::onMeshSelectedInMainScene()
{
    std::cout<<__func__<<std::endl;
    editButton->setStyleSheet(editButtonActiveStyleSheet);
}

void MainWindow::onMovedToEditScene()
{
    editButton->setStyleSheet(editButtonInactiveStyleSheet);
}
