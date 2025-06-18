#include "movetoothpanelwidget.h"

#include "QBoxLayout"
#include "QLabel"

MoveToothPanelWidget::MoveToothPanelWidget(QWidget *parent) : QWidget{parent}
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Move Tooth/Crown"));
    layout->addStretch();
}
