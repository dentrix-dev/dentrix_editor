#ifndef UNIFORMSCALEPANELWIDGET_H
#define UNIFORMSCALEPANELWIDGET_H

#include <QWidget>
#include <glwidget.h>

class UniformScalePanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UniformScalePanelWidget(GLWidget *glWidget);

signals:
};

#endif // UNIFORMSCALEPANELWIDGET_H
