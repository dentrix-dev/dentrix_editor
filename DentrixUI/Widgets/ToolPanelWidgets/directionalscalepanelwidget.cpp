#include "directionalscalepanelwidget.h"
#include "QBoxLayout"
#include "QSlider"
#include "QLabel"
#include "QCheckBox"


DirectionalScalePanelWidget::DirectionalScalePanelWidget(GLWidget *glWidget)
    : QWidget{nullptr}
{
    QVBoxLayout* viewLayout = new QVBoxLayout(this);
    viewLayout->addWidget(new QLabel("Directional Scale"));
    QSlider *directionalScaleSlider = new QSlider(Qt::Horizontal);
    directionalScaleSlider->setMinimum(5);    // Maps to 0.1
    directionalScaleSlider->setMaximum(20);   // Maps to 5.0
    directionalScaleSlider->setValue(10);     // e.g., 1.0
    viewLayout->addWidget(directionalScaleSlider);
    QCheckBox* xCheckBox = new QCheckBox("X");
    QCheckBox* yCheckBox = new QCheckBox("Y");
    QCheckBox* zCheckBox = new QCheckBox("Z");
    viewLayout->addWidget(xCheckBox);
    viewLayout->addWidget(yCheckBox);
    viewLayout->addWidget(zCheckBox);
    connect(directionalScaleSlider, &QSlider::sliderMoved, this, [=](int value) {
        bool xEnabled = xCheckBox->isChecked();
        bool yEnabled = yCheckBox->isChecked();
        bool zEnabled = zCheckBox->isChecked();

        glWidget->setSelectedMeshDirectionalScale(value, xEnabled, yEnabled, zEnabled);
    });
    viewLayout->addStretch();
}
