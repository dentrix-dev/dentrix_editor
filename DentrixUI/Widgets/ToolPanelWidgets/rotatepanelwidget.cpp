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
    rotateSlider->setMinimum(5);   // Maps to 0.5
    rotateSlider->setMaximum(15);  // Maps to 1.5
    rotateSlider->setValue(10);    // Maps to 1
    // connect(rotateSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setSelectedMeshScale);
    // connect(rotateSlider, &QSlider::sliderReleased, glWidget, &GLWidget::updateMeshScale);
    // connect(rotateSlider, &QSlider::sliderReleased, this, &UniformScalePanelWidget::resetSlider);
    editLayout->addWidget(rotateSlider);
    editLayout->addStretch();
}

void RotatePanelWidget::onQActionGroupTriggered(QAction *action)
{
    std::cout << __func__ << std::endl;
    rotateSlider->setValue(10);
}

void RotatePanelWidget::resetSlider()
{
    rotateSlider->setValue(10);
}
