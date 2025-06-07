#ifndef ROTATEPANELWIDGET_H
#define ROTATEPANELWIDGET_H

#include <QSlider>
#include <QWidget>

class RotatePanelWidget : public QWidget
{
    Q_OBJECT
    QSlider *rotateSlider;

public:
    explicit RotatePanelWidget(QWidget *parent = nullptr);

    QSlider *getRotateSlider() const;

signals:

public slots:
    void onQActionGroupTriggered(QAction *action);
    void resetSlider();
};

#endif  // ROTATEPANELWIDGET_H
