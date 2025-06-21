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
    
    QFrame* hintFrame = new QFrame();
    hintFrame->setFrameShape(QFrame::NoFrame);
    hintFrame->setStyleSheet(
        "QFrame {"
        "  background-color: #2D2F31;"
        "  border-radius: 3px;"
        "  padding: 2px 4px;"
        "}");
    QHBoxLayout* hintLayout = new QHBoxLayout(hintFrame);
    hintLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* hintIcon = new QLabel();
    hintIcon->setPixmap(
        QPixmap(":DentrixUI/Icons/hint.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* hintText = new QLabel("Hold shift to activate brush");
    hintText->setStyleSheet(
        "QLabel {"
        "  color: #E8EAED;"
        "  font-size: 10px;"
        "  font-weight: 500;"
        "  margin-right: 0px;"
        "}");
    hintText->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    hintText->setMaximumWidth(180);
    hintLayout->addWidget(hintIcon, 0, Qt::AlignVCenter);
    hintLayout->addWidget(hintText, 1);
    smoothingLayout->addWidget(hintFrame);
    smoothingLayout->addSpacing(8);

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
