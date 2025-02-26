#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , glWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Dentrix Editor");
    connect(ui->actionLoad_Model, &QAction::triggered, this, &MainWindow::loadModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadModel()
{
    std::cout<<"load model called"<<std::endl;
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "../../models", "Model Files (*.obj *.stl *.ply)");
    if (!filePath.isEmpty()) {
        glWidget = new GLWidget(this, filePath.toStdString());
        setCentralWidget(glWidget);
    }
}
