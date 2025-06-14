#include "loadingtoolbar.h"

#include <QIcon>

const QString LoadingToolBar::LOAD_UPPER_ACTION_TEXT = "Load Upper Jaw";
const QString LoadingToolBar::LOAD_LOWER_ACTION_TEXT = "Load Lower Jaw";
const QString LoadingToolBar::ARCHES_ALIGNMENT_ACTION_TEXT = "Arches Alignment";

LoadingToolBar::LoadingToolBar(QWidget* parent)
    : QToolBar("Loading Bar", parent)
{
    setMovable(false);
    setIconSize(QSize(24, 24));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    loadUpperAction = new QAction(QIcon(":/DentrixUI/Icons/load-upper.svg"), LOAD_UPPER_ACTION_TEXT, this);
    loadUpperAction->setToolTip(LOAD_UPPER_ACTION_TEXT);

    loadLowerAction = new QAction(QIcon(":/DentrixUI/Icons/load-lower.png"), LOAD_LOWER_ACTION_TEXT, this);
    loadLowerAction->setToolTip(LOAD_LOWER_ACTION_TEXT);

    alignJawsAction = new QAction(QIcon(":/DentrixUI/Icons/alignment.svg"), ARCHES_ALIGNMENT_ACTION_TEXT, this);
    alignJawsAction->setToolTip(ARCHES_ALIGNMENT_ACTION_TEXT);
    alignJawsAction->setEnabled(false);  // will be enabled whe two jaws loaded

    addAction(loadUpperAction);
    addAction(loadLowerAction);
    addSeparator();
    addAction(alignJawsAction);

    connect(loadUpperAction, &QAction::triggered, this, [this]() {
        emit loadUpperRequested();
    });
    
    connect(loadLowerAction, &QAction::triggered, this, [this]() {
        emit loadLowerRequested();
    });
    
    connect(alignJawsAction, &QAction::triggered, this, [this]() {
        emit alignJawsRequested();
    });
}

void LoadingToolBar::setAlignEnabled(bool enabled)
{
    alignJawsAction->setEnabled(enabled);
}
