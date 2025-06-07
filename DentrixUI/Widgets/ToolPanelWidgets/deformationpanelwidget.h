#ifndef DEFORMATIONPANELWIDGET_H
#define DEFORMATIONPANELWIDGET_H

#include <glwidget.h>

#include <QWidget>

#include "QButtonGroup"
#include "QSlider"

class DeformationPanelWidget : public QWidget
{
    Q_OBJECT

    QButtonGroup* modeGroup;
    QSlider* strengthSlider;
    QSlider* sizeSlider;

public:
    explicit DeformationPanelWidget();

    QButtonGroup* getModeGroup();

    QSlider* getStrengthSlider();

    QSlider* getSizeSlider();

signals:
};

#endif  // DEFORMATIONPANELWIDGET_H
