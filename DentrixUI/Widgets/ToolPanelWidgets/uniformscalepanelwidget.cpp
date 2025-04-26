#include "uniformscalepanelwidget.h"
#include "QBoxLayout"
#include "QSlider"
#include "QLabel"

UniformScalePanelWidget::UniformScalePanelWidget(GLWidget *glWidget)
    : QWidget{nullptr}
{
    QVBoxLayout* editLayout = new QVBoxLayout(this);
    editLayout->addWidget(new QLabel("Uniform Scale"));
    QSlider *uniformScaleSlider = new QSlider(Qt::Horizontal);
    uniformScaleSlider->setMinimum(5);    // Maps to 0.1
    uniformScaleSlider->setMaximum(20);   // Maps to 5.0
    uniformScaleSlider->setValue(10);     // e.g., 1.0
    connect(uniformScaleSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setSelectedMeshScale);
    connect(uniformScaleSlider, &QSlider::sliderReleased, glWidget, &GLWidget::updateMeshScale);
    editLayout->addWidget(uniformScaleSlider);
    editLayout->addStretch();
}
