#include "rightpanelstackedwidget.h"

#include <QLabel>
#include <QVBoxLayout>

RightPanelStackedWidget::RightPanelStackedWidget(GLWidget* glWidget) : QStackedWidget(nullptr)
{
    QWidget* emptyPanel = new QWidget();
    QVBoxLayout* emptyLayout = new QVBoxLayout(emptyPanel);
    emptyLayout->addWidget(new QLabel("No transformation is selected."));
    emptyLayout->addStretch();

    uniformScalePanel = new UniformScalePanelWidget(glWidget);

    directionalScalePanel = new DirectionalScalePanelWidget(glWidget);

    deformationPanel = new DeformationPanelWidget(glWidget);

    rotatePanel = new RotatePanelWidget();

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
