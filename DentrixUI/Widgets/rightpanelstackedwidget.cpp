#include "rightpanelstackedwidget.h"
#include "Widgets/ToolPanelWidgets/uniformscalepanelwidget.h"
#include "Widgets/ToolPanelWidgets/directionalscalepanelwidget.h"
#include "Widgets/ToolPanelWidgets/deformationpanelwidget.h"
#include <QVBoxLayout>
#include <QLabel>

RightPanelStackedWidget::RightPanelStackedWidget(GLWidget *glWidget):
    QStackedWidget(nullptr)
{
    QWidget* emptyPanel = new QWidget();
    QVBoxLayout* emptyLayout = new QVBoxLayout(emptyPanel);
    emptyLayout->addWidget(new QLabel("No transformation is selected."));
    emptyLayout->addStretch();


    UniformScalePanelWidget* uniformScalePanel = new UniformScalePanelWidget(glWidget);

    DirectionalScalePanelWidget* directionalScalePanel = new DirectionalScalePanelWidget(glWidget);

    DeformationPanelWidget* deformationPanel = new DeformationPanelWidget(glWidget);


    // Add panels to the stack
    this->addWidget(emptyPanel); // index 0
    this->addWidget(uniformScalePanel);  // index 1
    this->addWidget(directionalScalePanel);  // index 2
    this->addWidget(deformationPanel);  // index 3

    this->setFixedWidth(200);

}
