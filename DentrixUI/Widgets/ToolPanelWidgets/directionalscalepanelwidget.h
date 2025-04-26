#ifndef DIRECTIONALSCALEPANELWIDGET_H
#define DIRECTIONALSCALEPANELWIDGET_H

#include <QWidget>
#include <glwidget.h>

class DirectionalScalePanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DirectionalScalePanelWidget(GLWidget *glWidget);

signals:
};

#endif // DIRECTIONALSCALEPANELWIDGET_H
