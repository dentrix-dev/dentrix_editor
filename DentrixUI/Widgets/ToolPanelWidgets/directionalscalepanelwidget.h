#ifndef DIRECTIONALSCALEPANELWIDGET_H
#define DIRECTIONALSCALEPANELWIDGET_H

#include <QWidget>
#include <glwidget.h>
#include <QSlider>

class DirectionalScalePanelWidget : public QWidget
{
    Q_OBJECT
    QSlider *directionalScaleSlider;
public:
    explicit DirectionalScalePanelWidget(GLWidget *glWidget);


public slots:
    void onQActionGroupTriggered(QAction* action);

};

#endif // DIRECTIONALSCALEPANELWIDGET_H
