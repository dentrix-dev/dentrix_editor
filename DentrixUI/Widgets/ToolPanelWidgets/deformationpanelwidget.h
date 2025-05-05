#ifndef DEFORMATIONPANELWIDGET_H
#define DEFORMATIONPANELWIDGET_H

#include <glwidget.h>

#include <QWidget>

class DeformationPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeformationPanelWidget(GLWidget *glWidget);

signals:
};

#endif  // DEFORMATIONPANELWIDGET_H
