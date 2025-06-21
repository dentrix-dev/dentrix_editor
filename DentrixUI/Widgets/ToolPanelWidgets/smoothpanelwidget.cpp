#include "smoothpanelwidget.h"
#include "glwidget.h"

#include "QBoxLayout"
#include "QButtonGroup"
#include "QLabel"
#include "QPushButton"
#include "QSlider"

SmoothPanelWidget::SmoothPanelWidget(GLWidget* glWidget, QWidget* parent) 
    : QWidget{parent}, glWidget(glWidget)
{
    QVBoxLayout* smoothingLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* titleLabel = new QLabel("Smoothing Tool");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 10px;");
    smoothingLayout->addWidget(titleLabel);

    // Strength slider
    QLabel* strengthLabel = new QLabel("Strength");
    QSlider* strengthSlider = new QSlider(Qt::Horizontal);
    strengthSlider->setMinimum(1);
    strengthSlider->setMaximum(20);
    strengthSlider->setValue(10);
    smoothingLayout->addWidget(strengthLabel);
    smoothingLayout->addWidget(strengthSlider);
    connect(strengthSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setSmoothingStrength);

    // Brush size slider
    QLabel* sizeLabel = new QLabel("Brush Size");
    QSlider* sizeSlider = new QSlider(Qt::Horizontal);
    sizeSlider->setMinimum(5);
    sizeSlider->setMaximum(30);
    sizeSlider->setValue(10);
    smoothingLayout->addWidget(sizeLabel);
    smoothingLayout->addWidget(sizeSlider);
    connect(sizeSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setBrushSize);
    
    smoothingLayout->addStretch();
}
