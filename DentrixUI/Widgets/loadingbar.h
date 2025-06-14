#ifndef LOADINGBAR_H
#define LOADINGBAR_H

#include <QToolBar>
#include <QAction>
#include <iostream>

class LoadingBar : public QToolBar
{
    Q_OBJECT

public:
    explicit LoadingBar(QWidget* parent = nullptr);

    static const QString LOAD_UPPER_ACTION_TEXT;
    static const QString LOAD_LOWER_ACTION_TEXT;
    static const QString ALIGN_JAWS_ACTION_TEXT;

public slots:
    void setAlignEnabled(bool enabled);

signals:
    void loadUpperRequested();
    void loadLowerRequested();
    void alignJawsRequested();

private:
    QAction* loadUpperAction;
    QAction* loadLowerAction;
    QAction* alignJawsAction;
};

#endif  // LOADINGBAR_H
