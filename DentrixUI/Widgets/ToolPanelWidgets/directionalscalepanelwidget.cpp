#include "directionalscalepanelwidget.h"

#include "QBoxLayout"
#include "QCheckBox"
#include "QLabel"

DirectionalScalePanelWidget::DirectionalScalePanelWidget(GLWidget* glWidget) : QWidget{nullptr}
{
    QVBoxLayout* viewLayout = new QVBoxLayout(this);
    viewLayout->addWidget(new QLabel("Directional Scale"));
    directionalScaleSlider = new QSlider(Qt::Horizontal);
    directionalScaleSlider->setMinimum(5);   // Maps to 0.5
    directionalScaleSlider->setMaximum(15);  // Maps to 1.5
    directionalScaleSlider->setValue(10);    // Maps to 1.0
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
    connect(directionalScaleSlider, &QSlider::sliderReleased, this, [=, this]() { directionalScaleSlider->setValue(10); });
    viewLayout->addStretch();
}

void DirectionalScalePanelWidget::onQActionGroupTriggered(QAction* action)
{
    std::cout << __func__ << std::endl;
    directionalScaleSlider->setValue(10);
}
