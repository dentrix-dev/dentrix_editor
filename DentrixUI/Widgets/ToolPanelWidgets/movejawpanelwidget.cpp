#include "movejawpanelwidget.h"

#include "QBoxLayout"
#include "QLabel"
#include "QPushButton"

MoveJawPanelWidget::MoveJawPanelWidget(QWidget *parent) : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Move Jaw"));
    resetButton = new QPushButton("Reset Jaw Positions");
    layout->addWidget(resetButton);
    layout->addStretch();
}

QPushButton *MoveJawPanelWidget::getResetButton()
{
    return resetButton;
}
