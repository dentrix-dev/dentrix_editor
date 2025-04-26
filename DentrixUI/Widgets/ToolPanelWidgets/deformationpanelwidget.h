#ifndef DEFORMATIONPANELWIDGET_H
#define DEFORMATIONPANELWIDGET_H

#include <QWidget>
#include <glwidget.h>

class DeformationPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeformationPanelWidget(GLWidget *glWidget);

signals:
};

#endif // DEFORMATIONPANELWIDGET_H
