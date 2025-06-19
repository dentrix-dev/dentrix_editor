#ifndef MOVEJAWPANELWIDGET_H
#define MOVEJAWPANELWIDGET_H

#include <QWidget>

class QPushButton;

class MoveJawPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MoveJawPanelWidget(QWidget* parent = nullptr);

signals:

private:
    QPushButton* resetButton;
};

#endif  // MOVEJAWPANELWIDGET_H
