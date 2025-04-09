#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QToolBar>

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

    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addSeparator(); // Adds a separator line
    toolBar->addAction(exitAction);

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
