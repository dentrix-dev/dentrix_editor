#ifndef UNIFORMSCALEPANELWIDGET_H
#define UNIFORMSCALEPANELWIDGET_H

#include <glwidget.h>

#include <QWidget>

#include "QSlider"

class UniformScalePanelWidget : public QWidget
{
    Q_OBJECT
    QSlider *uniformScaleSlider;

public:
    explicit UniformScalePanelWidget(GLWidget *glWidget);

signals:

public slots:
    void onQActionGroupTriggered(QAction *action);
    void resetSlider();
};

#endif  // UNIFORMSCALEPANELWIDGET_H
