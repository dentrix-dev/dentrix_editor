#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QToolBar>
#include <QWidgetAction>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , glWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Dentrix Editor");
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

    // Add actions (buttons) to the toolbar
    QAction *openAction = new QAction("tool 1", this);
    QAction *saveAction = new QAction("tool 2", this);
    QAction *exitAction = new QAction("tool 3", this);
    // Add a spacer widget to push following widgets to the bottom
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidgetAction *spacerAction = new QWidgetAction(this);
    spacerAction->setDefaultWidget(spacer);
    // Create a styled blue "Edit" button
    QPushButton *editButton = new QPushButton("Edit");
    editButton->setStyleSheet("background-color: #3498db; color: white; padding: 5px;");
    // Wrap it in a QWidgetAction so it can go in the QToolBar
    QWidgetAction *editAction = new QWidgetAction(this);
    editAction->setDefaultWidget(editButton);

    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addSeparator(); // Adds a separator line
    toolBar->addAction(exitAction);
    toolBar->addAction(spacerAction); // Add it before the edit button
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
            setCentralWidget(glWidget);
        } else {
            glWidget->loadModel(filePath.toStdString());
        }
    }
}
