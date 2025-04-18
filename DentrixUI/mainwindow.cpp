#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QToolBar>
#include <QWidgetAction>
#include <iostream>
#include <QActionGroup>
#include <QBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>

bool MainWindow::inUnformScale = false;
bool MainWindow::inDirectionalScale = false;
bool MainWindow::inFreeDeformation = false;


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

    //Create Transformation Actions Group
    QActionGroup *actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    connect(actionGroup, &QActionGroup::triggered, this, &MainWindow::onQActionGroupTriggered);

    // Create actions (buttons) to add to the toolbar
    QAction *uniformTranslateAction = new QAction(UNIFORM_SCALE_ACTION_TEXT, this);
    uniformTranslateAction->setCheckable(true);
    actionGroup->addAction(uniformTranslateAction);
    QAction *saveAction = new QAction(DIRECTIONAL_SCALE_ACTION_TEXT, this);
    saveAction->setCheckable(true);
    actionGroup->addAction(saveAction);
    QAction *exitAction = new QAction(FREE_DEFORM_ACTION_TEXT, this);
    exitAction->setCheckable(true);
    actionGroup->addAction(exitAction);

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
    toolBar->addAction(exitAction);
    toolBar->addSeparator();
    toolBar->addAction(spacerAction);
    toolBar->addAction(editAction);

    // Add toolbar to the left side
    addToolBar(Qt::LeftToolBarArea, toolBar);
}

void MainWindow::createRightPanelStack()
{
    // ==== Right Panel Stack ====
    rightPanelStack = new QStackedWidget(this);

    // Panel 0: Empty Panel
    QWidget* emptyPanel = new QWidget();
    QVBoxLayout* emptyLayout = new QVBoxLayout(emptyPanel);
    emptyLayout->addWidget(new QLabel("No transformation is selected."));
    emptyLayout->addStretch();


    // Panel 1: Edit Mode
    QWidget* editPanel = new QWidget();
    QVBoxLayout* editLayout = new QVBoxLayout(editPanel);
    editLayout->addWidget(new QLabel("Uniform Scale"));
    QSlider *uniformScaleSlider = new QSlider(Qt::Horizontal);
    uniformScaleSlider->setMinimum(5);    // Maps to 0.1
    uniformScaleSlider->setMaximum(20);   // Maps to 5.0
    uniformScaleSlider->setValue(10);     // e.g., 1.0
    connect(uniformScaleSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setSelectedMeshScale);
    editLayout->addWidget(uniformScaleSlider);
    editLayout->addStretch();

    // Panel 2: View Mode
    QWidget* viewPanel = new QWidget();
    QVBoxLayout* viewLayout = new QVBoxLayout(viewPanel);
    viewLayout->addWidget(new QLabel("Directional Scale"));
    QSlider *directionalScaleSlider = new QSlider(Qt::Horizontal);
    directionalScaleSlider->setMinimum(5);    // Maps to 0.1
    directionalScaleSlider->setMaximum(20);   // Maps to 5.0
    directionalScaleSlider->setValue(10);     // e.g., 1.0
    viewLayout->addWidget(directionalScaleSlider);
    QCheckBox* xCheckBox = new QCheckBox("X");
    QCheckBox* yCheckBox = new QCheckBox("Y");
    QCheckBox* zCheckBox = new QCheckBox("Z");
    viewLayout->addWidget(xCheckBox);
    viewLayout->addWidget(yCheckBox);
    viewLayout->addWidget(zCheckBox);
    connect(directionalScaleSlider, &QSlider::sliderMoved, this, [=](int value) {
        bool xEnabled = xCheckBox->isChecked();
        bool yEnabled = yCheckBox->isChecked();
        bool zEnabled = zCheckBox->isChecked();

        glWidget->setSelectedMeshDirectionalScale(value, xEnabled, yEnabled, zEnabled);
    });
    viewLayout->addStretch();

    // Panel 3: Free Deformation Panel
    QWidget* deformationPanel = new QWidget();
    QVBoxLayout* deformationLayout = new QVBoxLayout(deformationPanel);
    QLabel* modeLabel = new QLabel("Mode");
    QPushButton* addButton = new QPushButton("Add");
    QPushButton* removeButton = new QPushButton("Remove");

    // add-remove
    addButton->setCheckable(true);
    removeButton->setCheckable(true);
    addButton->setChecked(true);  // Default state
    QButtonGroup* modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addButton(addButton, 0);
    modeGroup->addButton(removeButton, 1);
    QHBoxLayout* modeButtonLayout = new QHBoxLayout();
    modeButtonLayout->addWidget(addButton);
    modeButtonLayout->addWidget(removeButton);
    deformationLayout->addWidget(modeLabel);
    deformationLayout->addLayout(modeButtonLayout);
    connect(modeGroup, &QButtonGroup::idClicked, this, [=](int id){
        bool isAdd = (id == 0);
        glWidget->setFreeDeformAddMode(isAdd);
    });

    // Strength slider
    QLabel* strengthLabel = new QLabel("Strength");
    QSlider* strengthSlider = new QSlider(Qt::Horizontal);
    strengthSlider->setMinimum(5);
    strengthSlider->setMaximum(20);
    strengthSlider->setValue(10);
    deformationLayout->addWidget(strengthLabel);
    deformationLayout->addWidget(strengthSlider);
    connect(strengthSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setDeformationStrength);

    // Brush size slider
    QLabel* sizeLabel = new QLabel("Brush Size");
    QSlider* sizeSlider = new QSlider(Qt::Horizontal);
    sizeSlider->setMinimum(5);
    sizeSlider->setMaximum(20);
    sizeSlider->setValue(10);
    deformationLayout->addWidget(sizeLabel);
    deformationLayout->addWidget(sizeSlider);
    connect(sizeSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setBrushSize);
    deformationLayout->addStretch();


    // Add panels to the stack
    rightPanelStack->addWidget(emptyPanel); // index 0
    rightPanelStack->addWidget(editPanel);  // index 1
    rightPanelStack->addWidget(viewPanel);  // index 2
    rightPanelStack->addWidget(deformationPanel);  // index 3

    rightPanelStack->setFixedWidth(200);
}


void MainWindow::loadModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "../../models", "Model Files (*.obj *.stl *.ply)");
    if (!filePath.isEmpty()) {
        if (!glWidget){
            glWidget = new GLWidget(this, filePath.toStdString());
            connect(glWidget, &GLWidget::meshSelectedInMainScene, this, &MainWindow::onMeshSelectedInMainScene);
            connect(glWidget, &GLWidget::movedToEditScene, this, &MainWindow::onMovedToEditScene);
            connect(glWidget, &GLWidget::movedToMainScene, this, &MainWindow::onMovedToMainScene);
            connect(editButton, &QPushButton::clicked, glWidget, &GLWidget::onEditSceneClicked);

            // ... inside the if (!glWidget) section
            // Create a horizontal layout for the central widget
            QWidget* centralContainer = new QWidget(this);
            QHBoxLayout* centralLayout = new QHBoxLayout(centralContainer);
            centralLayout->setContentsMargins(0, 0, 0, 0);

            // Add the glWidget to the left
            centralLayout->addWidget(glWidget, 1);

            createRightPanelStack();

            // Add to layout
            centralLayout->addWidget(rightPanelStack);

            // Set the container as the central widget
            setCentralWidget(centralContainer);

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
    // editButton->setStyleSheet(editButtonInactiveStyleSheet);
    // edit button becomes save and return
    editButton->setText("Save");
    editButton->setToolTip("Save and return to main scene");
}

void MainWindow::onMovedToMainScene()
{
    // editButton->setStyleSheet(editButtonInactiveStyleSheet);
    editButton->setText("Edit");
}

void MainWindow::onQActionGroupTriggered(QAction *action)
{
    QString actionText = action->text();
    if (actionText == UNIFORM_SCALE_ACTION_TEXT){
        MainWindow::inUnformScale = true;
        MainWindow::inDirectionalScale = false;
        MainWindow::inFreeDeformation = false;
        rightPanelStack->setCurrentIndex(1);
    } else if (actionText == DIRECTIONAL_SCALE_ACTION_TEXT){
        MainWindow::inUnformScale = false;
        MainWindow::inDirectionalScale = true;
        MainWindow::inFreeDeformation = false;
        rightPanelStack->setCurrentIndex(2);
    } else if (actionText == FREE_DEFORM_ACTION_TEXT){
        MainWindow::inUnformScale = false;
        MainWindow::inDirectionalScale = false;
        MainWindow::inFreeDeformation = true;
        rightPanelStack->setCurrentIndex(3);
    }
    if (glWidget) {
        glWidget->updateCursor();
    }
}