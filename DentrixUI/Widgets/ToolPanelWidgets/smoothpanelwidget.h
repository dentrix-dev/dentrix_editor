#ifndef SMOOTHPANELWIDGET_H
#define SMOOTHPANELWIDGET_H

#include <QWidget>

class GLWidget;

class SmoothPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SmoothPanelWidget(GLWidget* glWidget, QWidget *parent = nullptr);

signals:

private:
    GLWidget* glWidget;
};

#endif  // SMOOTHPANELWIDGET_H
