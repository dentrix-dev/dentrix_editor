#include "deformationpanelwidget.h"
#include "QBoxLayout"
#include "QButtonGroup"
#include "QLabel"
#include "QPushButton"
#include "QSlider"
#include "QFrame"
#include "QFont"

DeformationPanelWidget::DeformationPanelWidget(GLWidget* glWidget) : QWidget{nullptr}
{
    QVBoxLayout* deformationLayout = new QVBoxLayout(this);

    QFrame* hintFrame = new QFrame();
    hintFrame->setFrameShape(QFrame::NoFrame);
    hintFrame->setStyleSheet(
        "QFrame {"
        "  background-color: #2D2F31;"
        "  border-radius: 3px;"
        "  padding: 2px 4px;"
        "}"
        );
    QHBoxLayout* hintLayout = new QHBoxLayout(hintFrame);
    hintLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* hintIcon = new QLabel();
    hintIcon->setPixmap(QPixmap(":DentrixUI/Icons/hint.png").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* hintText = new QLabel("Hold shift to activate brush");
    hintText->setStyleSheet(
        "QLabel {"
        "  color: #E8EAED;"
        "  font-size: 10px;"
        "  font-weight: 500;"
        "  margin-right: 0px;"
        "}"
        );
    hintText->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    hintText->setMaximumWidth(180);
    hintLayout->addWidget(hintIcon, 0, Qt::AlignVCenter);
    hintLayout->addWidget(hintText, 1);
    deformationLayout->addWidget(hintFrame);
    deformationLayout->addSpacing(8);

    // Original content
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
    connect(modeGroup, &QButtonGroup::idClicked, this, [=](int id) {
        bool isAdd = (id == 0);
        glWidget->setFreeDeformAddMode(isAdd);
    });
    // Strength slider
    QLabel* strengthLabel = new QLabel("Strength");
    QSlider* strengthSlider = new QSlider(Qt::Horizontal);
    strengthSlider->setMinimum(5);
    strengthSlider->setMaximum(20);
    strengthSlider->setValue(10);
    deformationLayout->addWidget(strengthLabel);
    deformationLayout->addWidget(strengthSlider);
    connect(strengthSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setDeformationStrength);
    // Brush size slider
    QLabel* sizeLabel = new QLabel("Brush Size");
    QSlider* sizeSlider = new QSlider(Qt::Horizontal);
    sizeSlider->setMinimum(5);
    sizeSlider->setMaximum(20);
    sizeSlider->setValue(10);
    deformationLayout->addWidget(sizeLabel);
    deformationLayout->addWidget(sizeSlider);
    connect(sizeSlider, &QSlider::sliderMoved, glWidget, &GLWidget::setBrushSize);
    deformationLayout->addStretch();
}
