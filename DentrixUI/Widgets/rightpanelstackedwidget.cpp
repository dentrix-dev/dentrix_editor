#include "rightpanelstackedwidget.h"

#include <QLabel>
#include <QVBoxLayout>

RightPanelStackedWidget::RightPanelStackedWidget(GLWidget* glWidget) : QStackedWidget(nullptr)
{
    this->glWidget = glWidget;
    QWidget* emptyPanel = new QWidget();
    QVBoxLayout* emptyLayout = new QVBoxLayout(emptyPanel);
    emptyLayout->addWidget(new QLabel("No transformation is selected."));
    emptyLayout->addStretch();

    uniformScalePanel = new UniformScalePanelWidget(glWidget);

    directionalScalePanel = new DirectionalScalePanelWidget(glWidget);

    deformationPanel = new DeformationPanelWidget();
    initDeformationPanelSignals();

    rotatePanel = new RotatePanelWidget();
    initRotatePanelSignals();

    smoothPanel = new SmoothPanelWidget();

    // Add panels to the stack
    this->addWidget(emptyPanel);             // index 0
    this->addWidget(uniformScalePanel);      // index 1
    this->addWidget(directionalScalePanel);  // index 2
    this->addWidget(deformationPanel);       // index 3
    this->addWidget(rotatePanel);            // index 4
    this->addWidget(smoothPanel);

    this->setFixedWidth(200);
}

void RightPanelStackedWidget::initDeformationPanelSignals()
{
    connect(deformationPanel->getModeGroup(), &QButtonGroup::idClicked, deformationPanel, [=](int id) {
        bool isAdd = (id == 0);
        glWidget->setFreeDeformAddMode(isAdd);
    });
    connect(deformationPanel->getStrengthSlider(), &QSlider::sliderMoved, glWidget, &GLWidget::setDeformationStrength);
    connect(deformationPanel->getSizeSlider(), &QSlider::sliderMoved, glWidget, &GLWidget::setBrushSize);
}

void RightPanelStackedWidget::initRotatePanelSignals()
{
    QSlider* rotateSlider = rotatePanel->getRotateSlider();
    connect(rotateSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setSelectedMeshRotationAngle);
    connect(rotateSlider, &QSlider::sliderReleased, glWidget, &GLWidget::rotateMesh);
    connect(rotateSlider, &QSlider::sliderReleased, rotatePanel, &RotatePanelWidget::resetSlider);
}
