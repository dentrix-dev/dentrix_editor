#include "mainwindow.h"

#include <qnamespace.h>

#include <QActionGroup>
#include <QBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSlider>
#include <QToolBar>
#include <QWidgetAction>
#include <iostream>

#include "./ui_mainwindow.h"
#include "Widgets/Dialogs/savemodeldialog.h"
#include "filehandler.h"
#include "utils.h"

// bool MainWindow::inUnformScale = false;
// bool MainWindow::inDirectionalScale = false;
// bool MainWindow::inFreeDeformation = false;
// bool MainWindow::inRotate = false;
// bool MainWindow::inMoveTooth = false;
// bool MainWindow::inMoveJaw = false;
MainWindow::ToolMode MainWindow::toolMode = MainWindow::None;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), glWidget(nullptr), upperLoaded(false), lowerLoaded(false)
{
    ui->setupUi(this);
    setWindowTitle("Dentrix Editor");
    resize(QApplication::primaryScreen()->geometry().width(), QApplication::primaryScreen()->geometry().height());
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
    Q_ASSERT(rightPanelStack->moveJawPanel);

    connect(toolbar->getActionGroup(), &QActionGroup::triggered, rightPanelStack->uniformScalePanel,
            &UniformScalePanelWidget::onQActionGroupTriggered);
    connect(toolbar->getActionGroup(), &QActionGroup::triggered, rightPanelStack->directionalScalePanel,
            &DirectionalScalePanelWidget::onQActionGroupTriggered);
    connect(rightPanelStack->moveJawPanel->getResetButton(), &QPushButton::clicked, glWidget,
            &GLWidget::onResetJawClicked);

    centralLayout->addWidget(rightPanelStack);
    setCentralWidget(centralContainer);
}

void MainWindow::saveModel()
{
    std::cout << "hello from save model";
}

void MainWindow::loadUpperJaw()
{
    std::cout << "Loading upper jaw..." << std::endl;
    QString file = QFileDialog::getOpenFileName(this, "Load Upper Jaw", "../../models", "Model Files (*.stl *.obj)");
    if (file.isEmpty()) return;

    if (!glWidget) {
        std::cout << "setup central ui" << std::endl;
        setupCentralUI(file.toStdString(), 1);
    } else {
        std::cout << "loaded upper jaw async" << std::endl;
        glWidget->loadUpperJawAsync(file);
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
        glWidget->loadLowerJawAsync(file);
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
        glWidget->archAlignAsync();
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
    if (actionText == ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT)
        MainWindow::toolMode = UniformScale;
    else if (actionText == ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT)
        MainWindow::toolMode = DirectionalScale;
    else if (actionText == ToolBarWidget::FREE_DEFORM_ACTION_TEXT)
        MainWindow::toolMode = FreeDeformation;
    else if (actionText == ToolBarWidget::ROTATE_ACTION_TEXT)
        MainWindow::toolMode = Rotate;
    else if (actionText == ToolBarWidget::MOVE_TOOTH_ACTION_TEXT)
        MainWindow::toolMode = MoveTooth;
    else if (actionText == ToolBarWidget::MOVE_JAW_ACTION_TEXT)
        MainWindow::toolMode = MoveJaw;

    if (rightPanelStack) {
        if (MainWindow::toolMode == UniformScale)
            rightPanelStack->setCurrentIndex(1);
        else if (MainWindow::toolMode == DirectionalScale)
            rightPanelStack->setCurrentIndex(2);
        else if (MainWindow::toolMode == FreeDeformation)
            rightPanelStack->setCurrentIndex(3);
        else if (MainWindow::toolMode == Rotate)
            rightPanelStack->setCurrentIndex(4);
        else if (MainWindow::toolMode == MoveTooth)
            rightPanelStack->setCurrentIndex(6);
        else if (MainWindow::toolMode == MoveJaw)
            rightPanelStack->setCurrentIndex(7);
    }

    if (glWidget) glWidget->onToolChange();
}

void MainWindow::on_actionSave_Model_triggered()
{
    SaveModelDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted || glWidget == nullptr) return;

    QString savePath = dialog.getPath();
    // Remove .obj as it will be added to the specific jaw filename
    if (savePath.endsWith(".obj", Qt::CaseInsensitive)) savePath.chop(4);
    std::cout << "Saving to: " << savePath.toStdString() << std::endl;

    // Meshes are merged into one as PMP doesn't support writing multiple meshes
    // If an existing file with the same name exists, is it overwritten
    pmp::SurfaceMesh mergedMesh;
    if (glWidget->upperJawLoaded) {
        mergedMesh = Utils::mergeMeshes(glWidget->upperJawMeshes);

        std::string filePath = (savePath + "_upper.obj").toStdString();
        FileHandler::writeMesh(filePath, mergedMesh);
        std::cout << "Saved upper jaw" << std::endl;
    }
    if (glWidget->lowerJawLoaded) {
        mergedMesh = Utils::mergeMeshes(glWidget->lowerJawMeshes);

        std::string filePath = (savePath + "_lower.obj").toStdString();
        FileHandler::writeMesh(filePath, mergedMesh);
        std::cout << "Saved lower jaw" << std::endl;
    }

    QMessageBox::information(this, "Saved", "File would be saved to:\n" + savePath);
}
