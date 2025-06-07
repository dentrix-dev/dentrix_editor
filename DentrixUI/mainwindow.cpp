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

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), glWidget(nullptr)
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
}

void MainWindow::loadModel()
{
    QString filePath =
        QFileDialog::getOpenFileName(this, "Open Model File", "../../models", "Model Files (*.obj *.stl *.ply)");
    if (!filePath.isEmpty()) {
        if (!glWidget) {
            glWidget = new GLWidget(this, filePath.toStdString());
            connect(glWidget, &GLWidget::meshSelectedInMainScene, this, &MainWindow::onMeshSelectedInMainScene);
            connect(glWidget, &GLWidget::movedToEditScene, this, &MainWindow::onMovedToEditScene);
            connect(glWidget, &GLWidget::movedToMainScene, this, &MainWindow::onMovedToMainScene);
            connect(toolbar->getEditButton(), &QPushButton::clicked, glWidget, &GLWidget::onEditSceneClicked);

            // ... inside the if (!glWidget) section
            // Create a horizontal layout for the central widget
            QWidget* centralContainer = new QWidget(this);
            QHBoxLayout* centralLayout = new QHBoxLayout(centralContainer);
            centralLayout->setContentsMargins(0, 0, 0, 0);

            // Add the glWidget to the left
            centralLayout->addWidget(glWidget, 1);

            rightPanelStack = new RightPanelStackedWidget(glWidget);
            Q_ASSERT(rightPanelStack->uniformScalePanel);
            Q_ASSERT(rightPanelStack->directionalScalePanel);
            connect(toolbar->getActionGroup(), &QActionGroup::triggered, rightPanelStack->uniformScalePanel,
                    &UniformScalePanelWidget::onQActionGroupTriggered);
            connect(toolbar->getActionGroup(), &QActionGroup::triggered, rightPanelStack->directionalScalePanel,
                    &DirectionalScalePanelWidget::onQActionGroupTriggered);

            // Add to layout
            centralLayout->addWidget(rightPanelStack);

            // Set the container as the central widget
            setCentralWidget(centralContainer);
        } else {
            glWidget->loadModel(filePath.toStdString());
            toolbar->set_edit_button_mode(ToolBarWidget::ButtonMode::Main_mode);
            toolbar->set_edit_button_active(false);
        }
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
    if (actionText == ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT) {
        MainWindow::inUnformScale = true;
        MainWindow::inDirectionalScale = false;
        MainWindow::inFreeDeformation = false;
        MainWindow::inRotate = false;
        rightPanelStack->setCurrentIndex(1);
    } else if (actionText == ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT) {
        MainWindow::inUnformScale = false;
        MainWindow::inDirectionalScale = true;
        MainWindow::inFreeDeformation = false;
        MainWindow::inRotate = false;
        rightPanelStack->setCurrentIndex(2);
    } else if (actionText == ToolBarWidget::FREE_DEFORM_ACTION_TEXT) {
        MainWindow::inUnformScale = false;
        MainWindow::inDirectionalScale = false;
        MainWindow::inFreeDeformation = true;
        MainWindow::inRotate = false;
        rightPanelStack->setCurrentIndex(3);
    } else if (actionText == ToolBarWidget::ROTATE_ACTION_TEXT) {
        MainWindow::inUnformScale = false;
        MainWindow::inDirectionalScale = false;
        MainWindow::inFreeDeformation = false;
        MainWindow::inRotate = true;
        rightPanelStack->setCurrentIndex(4);
    }
    if (glWidget) {
        glWidget->updateCursor();
    }
}
