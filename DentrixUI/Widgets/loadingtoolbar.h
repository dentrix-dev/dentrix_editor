#ifndef LOADINGTOOLBAR_H
#define LOADINGTOOLBAR_H

#include <QToolBar>
#include <QAction>
#include <iostream>

class LoadingToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit LoadingToolBar(QWidget* parent = nullptr);

    static const QString LOAD_UPPER_ACTION_TEXT;
    static const QString LOAD_LOWER_ACTION_TEXT;
    static const QString ARCHES_ALIGNMENT_ACTION_TEXT;

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

#endif
