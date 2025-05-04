#include "uniformscalepanelwidget.h"

#include "QBoxLayout"
#include "QLabel"

UniformScalePanelWidget::UniformScalePanelWidget(GLWidget *glWidget) : QWidget{nullptr}
{
	QVBoxLayout *editLayout = new QVBoxLayout(this);
	editLayout->addWidget(new QLabel("Uniform Scale"));
	uniformScaleSlider = new QSlider(Qt::Horizontal);
	uniformScaleSlider->setMinimum(5);   // Maps to 0.5
	uniformScaleSlider->setMaximum(15);  // Maps to 1.5
	uniformScaleSlider->setValue(10);    // Maps to 1
	connect(uniformScaleSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setSelectedMeshScale);
	connect(uniformScaleSlider, &QSlider::sliderReleased, glWidget, &GLWidget::updateMeshScale);
	connect(uniformScaleSlider, &QSlider::sliderReleased, this, &UniformScalePanelWidget::resetSlider);
	editLayout->addWidget(uniformScaleSlider);
	editLayout->addStretch();
}

void UniformScalePanelWidget::onQActionGroupTriggered(QAction *action)
{
	std::cout << __func__ << std::endl;
	uniformScaleSlider->setValue(10);
}

void UniformScalePanelWidget::resetSlider()
{
	uniformScaleSlider->setValue(10);
}
