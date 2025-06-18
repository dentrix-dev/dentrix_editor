#include "mainwindow.h"

#include <QActionGroup>
#include <QBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QSlider>
#include <QToolBar>
#include <QWidgetAction>
#include <iostream>

#include "./ui_mainwindow.h"

bool MainWindow::inUnformScale = false;
bool MainWindow::inDirectionalScale = false;
bool MainWindow::inFreeDeformation = false;
bool MainWindow::inRotate = false;
bool MainWindow::inMoveTooth = false;
bool MainWindow::inMoveJaw = false;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), glWidget(nullptr), upperLoaded(false), lowerLoaded(false)
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
    toolbar = new ToolBarWidget();
    addToolBar(Qt::LeftToolBarArea, toolbar);
    connect(toolbar->getActionGroup(), &QActionGroup::triggered, this, &MainWindow::onQActionGroupTriggered);

    loadingToolBar = new LoadingToolBar(this);
    addToolBar(Qt::TopToolBarArea, loadingToolBar);
    connect(loadingToolBar, &LoadingToolBar::loadUpperRequested, this, &MainWindow::loadUpperJaw);
    connect(loadingToolBar, &LoadingToolBar::loadLowerRequested, this, &MainWindow::loadLowerJaw);
    connect(loadingToolBar, &LoadingToolBar::alignJawsRequested, this, &MainWindow::alignJaws);
}

void MainWindow::setupCentralUI(const std::string& modelPath, int loadMode)
{
    glWidget = new GLWidget(this, modelPath, loadMode);
    connect(glWidget, &GLWidget::meshSelectedInMainScene, this, &MainWindow::onMeshSelectedInMainScene);
    connect(glWidget, &GLWidget::movedToEditScene, this, &MainWindow::onMovedToEditScene);
    connect(glWidget, &GLWidget::movedToMainScene, this, &MainWindow::onMovedToMainScene);
    connect(toolbar->getEditButton(), &QPushButton::clicked, glWidget, &GLWidget::onEditSceneClicked);

    QWidget* centralContainer = new QWidget(this);
    QHBoxLayout* centralLayout = new QHBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    centralLayout->addWidget(glWidget, 1);

    rightPanelStack = new RightPanelStackedWidget(glWidget);
    Q_ASSERT(rightPanelStack->uniformScalePanel);
    Q_ASSERT(rightPanelStack->directionalScalePanel);

    connect(toolbar->getActionGroup(), &QActionGroup::triggered, rightPanelStack->uniformScalePanel,
            &UniformScalePanelWidget::onQActionGroupTriggered);
    connect(toolbar->getActionGroup(), &QActionGroup::triggered, rightPanelStack->directionalScalePanel,
            &DirectionalScalePanelWidget::onQActionGroupTriggered);

    centralLayout->addWidget(rightPanelStack);
    setCentralWidget(centralContainer);
}

void MainWindow::loadModel()
{
    QString filePath =
        QFileDialog::getOpenFileName(this, "Open Model File", "../../models", "Model Files (*.obj *.stl *.ply)");
    if (filePath.isEmpty()) return;

    if (!glWidget) {
        setupCentralUI(filePath.toStdString(), 0);
    } else {
        glWidget->loadModel(filePath.toStdString());
        toolbar->set_edit_button_mode(ToolBarWidget::ButtonMode::Main_mode);
        toolbar->set_edit_button_active(false);
    }
}

void MainWindow::loadUpperJaw()
{
    std::cout << "Loading upper jaw..." << std::endl;
    QString file = QFileDialog::getOpenFileName(this, "Load Upper Jaw", "../../models", "Model Files (*.stl *.obj)");
    if (file.isEmpty()) return;

    if (!glWidget) {
        setupCentralUI(file.toStdString(), 1);
    } else {
        glWidget->loadUpperJaw(file.toStdString());
    }
    upperLoaded = true;
    loadingToolBar->setAlignEnabled(upperLoaded && lowerLoaded);
    toolbar->set_edit_button_mode(ToolBarWidget::Main_mode);
    toolbar->set_edit_button_active(false);
}

void MainWindow::loadLowerJaw()
{
    std::cout << "Loading lower jaw..." << std::endl;
    QString file = QFileDialog::getOpenFileName(this, "Load Lower Jaw", "../../models", "Model Files (*.stl *.obj)");
    if (file.isEmpty()) return;

    if (!glWidget) {
        setupCentralUI(file.toStdString(), 0);
    } else {
        glWidget->loadLowerJaw(file.toStdString());
    }
    lowerLoaded = true;
    loadingToolBar->setAlignEnabled(upperLoaded && lowerLoaded);
    toolbar->set_edit_button_mode(ToolBarWidget::Main_mode);
    toolbar->set_edit_button_active(false);
}

void MainWindow::alignJaws()
{
    std::cout << "Aligning jaws..." << std::endl;
    if (glWidget) {
        glWidget->archAlign();
        std::cout << "Jaws alignment requested" << std::endl;
    }
}

void MainWindow::onMeshSelectedInMainScene()
{
    std::cout << __func__ << std::endl;
    toolbar->set_edit_button_active();
}

void MainWindow::onMovedToEditScene()
{
    toolbar->set_edit_button_mode(ToolBarWidget::Edit_mode);
}

void MainWindow::onMovedToMainScene()
{
    toolbar->set_edit_button_mode(ToolBarWidget::Main_mode);
}

void MainWindow::onQActionGroupTriggered(QAction* action)
{
    QString actionText = action->text();
    inUnformScale = (actionText == ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT);
    inDirectionalScale = (actionText == ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT);
    inFreeDeformation = (actionText == ToolBarWidget::FREE_DEFORM_ACTION_TEXT);
    inRotate = (actionText == ToolBarWidget::ROTATE_ACTION_TEXT);
    inMoveTooth = (actionText == ToolBarWidget::MOVE_TOOTH_ACTION_TEXT);
    inMoveJaw = (actionText == ToolBarWidget::MOVE_JAW_ACTION_TEXT);

    if (rightPanelStack) {
        if (inUnformScale)
            rightPanelStack->setCurrentIndex(1);
        else if (inDirectionalScale)
            rightPanelStack->setCurrentIndex(2);
        else if (inFreeDeformation)
            rightPanelStack->setCurrentIndex(3);
        else if (inRotate)
            rightPanelStack->setCurrentIndex(4);
        else if (inMoveTooth)
            rightPanelStack->setCurrentIndex(6);
        else if (inMoveJaw)
            rightPanelStack->setCurrentIndex(7);
    }

    if (glWidget) glWidget->onToolChange();
}
