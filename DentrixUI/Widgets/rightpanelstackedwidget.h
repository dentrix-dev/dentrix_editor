#ifndef RIGHTPANELSTACKEDWIDGET_H
#define RIGHTPANELSTACKEDWIDGET_H
#include "QStackedWidget"
#include "glwidget.h"
#include "Widgets/ToolPanelWidgets/uniformscalepanelwidget.h"
#include "Widgets/ToolPanelWidgets/directionalscalepanelwidget.h"
#include "Widgets/ToolPanelWidgets/deformationpanelwidget.h"


class RightPanelStackedWidget : public QStackedWidget
{
public:
    RightPanelStackedWidget(GLWidget *glWidget);

    UniformScalePanelWidget* uniformScalePanel = nullptr;

    DirectionalScalePanelWidget* directionalScalePanel = nullptr;

    DeformationPanelWidget* deformationPanel = nullptr;

};

#endif // RIGHTPANELSTACKEDWIDGET_H
