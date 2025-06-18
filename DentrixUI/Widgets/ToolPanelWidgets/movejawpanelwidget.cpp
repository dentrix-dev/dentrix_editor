#include "movejawpanelwidget.h"

#include "QBoxLayout"
#include "QLabel"

MoveJawPanelWidget::MoveJawPanelWidget(QWidget *parent) : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Move Jaw"));
    layout->addStretch();
}
