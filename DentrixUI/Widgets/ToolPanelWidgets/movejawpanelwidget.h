#ifndef MOVEJAWPANELWIDGET_H
#define MOVEJAWPANELWIDGET_H

#include <QWidget>

class QPushButton;

class MoveJawPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MoveJawPanelWidget(QWidget* parent = nullptr);

    QPushButton* getResetButton();

signals:

private:
    QPushButton* resetButton;
};

#endif  // MOVEJAWPANELWIDGET_H
