#ifndef RIGHTPANELSTACKEDWIDGET_H
#define RIGHTPANELSTACKEDWIDGET_H
#include "QStackedWidget"
#include "Widgets/ToolPanelWidgets/deformationpanelwidget.h"
#include "Widgets/ToolPanelWidgets/directionalscalepanelwidget.h"
#include "Widgets/ToolPanelWidgets/rotatepanelwidget.h"
#include "Widgets/ToolPanelWidgets/smoothpanelwidget.h"
#include "Widgets/ToolPanelWidgets/uniformscalepanelwidget.h"
#include "glwidget.h"

class RightPanelStackedWidget : public QStackedWidget
{
public:
    RightPanelStackedWidget(GLWidget* glWidget);

    UniformScalePanelWidget* uniformScalePanel = nullptr;

    DirectionalScalePanelWidget* directionalScalePanel = nullptr;

    DeformationPanelWidget* deformationPanel = nullptr;

    SmoothPanelWidget* smoothPanel = nullptr;

    RotatePanelWidget* rotatePanel = nullptr;

private:
    void initDeformationPanelSignals();
    void initRotatePanelSignals();

    GLWidget* glWidget = nullptr;
};

#endif  // RIGHTPANELSTACKEDWIDGET_H
