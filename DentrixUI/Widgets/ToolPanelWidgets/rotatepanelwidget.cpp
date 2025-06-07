#include "rotatepanelwidget.h"

#include "QBoxLayout"
#include "QLabel"
#include "iostream"

QSlider *RotatePanelWidget::getRotateSlider() const
{
    return rotateSlider;
}

RotatePanelWidget::RotatePanelWidget(QWidget *parent) : QWidget{parent}
{
    QVBoxLayout *editLayout = new QVBoxLayout(this);
    editLayout->addWidget(new QLabel("Rotate"));
    rotateSlider = new QSlider(Qt::Horizontal);
    rotateSlider->setMinimum(-360);  // Maps to 0.5
    rotateSlider->setMaximum(360);   // Maps to 1.5
    rotateSlider->setValue(0);       // Maps to 1
    editLayout->addWidget(rotateSlider);
    editLayout->addStretch();
}

void RotatePanelWidget::onQActionGroupTriggered(QAction *action)
{
    std::cout << __func__ << std::endl;
    rotateSlider->setValue(0);
}

void RotatePanelWidget::resetSlider()
{
    rotateSlider->setValue(0);
}
