#ifndef RIGHTPANELSTACKEDWIDGET_H
#define RIGHTPANELSTACKEDWIDGET_H
#include "QStackedWidget"
#include "glwidget.h"

class RightPanelStackedWidget : public QStackedWidget
{
public:
    RightPanelStackedWidget(GLWidget *glWidget);
};

#endif // RIGHTPANELSTACKEDWIDGET_H
