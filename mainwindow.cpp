#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , glWidget(new GLWidget(this))
{
    ui->setupUi(this);
    setCentralWidget(glWidget);
    setWindowTitle("Dentrix Editor");
}

MainWindow::~MainWindow()
{
    delete ui;
}
