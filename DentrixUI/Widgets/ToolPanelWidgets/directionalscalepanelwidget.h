#ifndef DIRECTIONALSCALEPANELWIDGET_H
#define DIRECTIONALSCALEPANELWIDGET_H

#include <glwidget.h>

#include <QSlider>
#include <QWidget>

class DirectionalScalePanelWidget : public QWidget
{
    Q_OBJECT
    QSlider *directionalScaleSlider;

public:
    explicit DirectionalScalePanelWidget(GLWidget *glWidget);

public slots:
    void onQActionGroupTriggered(QAction *action);
};

#endif  // DIRECTIONALSCALEPANELWIDGET_H
