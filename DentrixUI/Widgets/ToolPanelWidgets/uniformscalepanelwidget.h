#ifndef UNIFORMSCALEPANELWIDGET_H
#define UNIFORMSCALEPANELWIDGET_H

#include <QWidget>
#include <glwidget.h>
#include "QSlider"

class UniformScalePanelWidget : public QWidget
{
    Q_OBJECT
    QSlider *uniformScaleSlider;
public:
    explicit UniformScalePanelWidget(GLWidget *glWidget);

signals:

public slots:
    void onQActionGroupTriggered(QAction* action);
    void resetSlider();

};

#endif // UNIFORMSCALEPANELWIDGET_H
