#include "smoothpanelwidget.h"

#include "QBoxLayout"
#include "QButtonGroup"
#include "QLabel"
#include "QPushButton"
#include "QSlider"

SmoothPanelWidget::SmoothPanelWidget(QWidget* parent) : QWidget{parent}
{
	QVBoxLayout* deformationLayout = new QVBoxLayout(this);
	QLabel* modeLabel = new QLabel("Mode");
	QPushButton* addButton = new QPushButton("Add");
	QPushButton* removeButton = new QPushButton("Remove");

	// add-remove
	addButton->setCheckable(true);
	removeButton->setCheckable(true);
	addButton->setChecked(true);  // Default state
	QButtonGroup* modeGroup = new QButtonGroup(this);
	modeGroup->setExclusive(true);
	modeGroup->addButton(addButton, 0);
	modeGroup->addButton(removeButton, 1);
	QHBoxLayout* modeButtonLayout = new QHBoxLayout();
	modeButtonLayout->addWidget(addButton);
	modeButtonLayout->addWidget(removeButton);
	deformationLayout->addWidget(modeLabel);
	deformationLayout->addLayout(modeButtonLayout);
	// connect(modeGroup, &QButtonGroup::idClicked, this, [=](int id){
	// 	bool isAdd = (id == 0);
	// 	glWidget->setFreeDeformAddMode(isAdd);
	// });

	// Strength slider
	QLabel* strengthLabel = new QLabel("Strength");
	QSlider* strengthSlider = new QSlider(Qt::Horizontal);
	strengthSlider->setMinimum(5);
	strengthSlider->setMaximum(20);
	strengthSlider->setValue(10);
	deformationLayout->addWidget(strengthLabel);
	deformationLayout->addWidget(strengthSlider);
	// connect(strengthSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setDeformationStrength);

	// Brush size slider
	QLabel* sizeLabel = new QLabel("Brush Size");
	QSlider* sizeSlider = new QSlider(Qt::Horizontal);
	sizeSlider->setMinimum(5);
	sizeSlider->setMaximum(20);
	sizeSlider->setValue(10);
	deformationLayout->addWidget(sizeLabel);
	deformationLayout->addWidget(sizeSlider);
	// connect(sizeSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setBrushSize);
	deformationLayout->addStretch();
}
