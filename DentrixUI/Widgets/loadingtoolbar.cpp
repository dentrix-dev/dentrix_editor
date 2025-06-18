#include "loadingtoolbar.h"

#include <QIcon>

const QString LoadingToolBar::LOAD_UPPER_ACTION_TEXT = "Load\nUpper Jaw";
const QString LoadingToolBar::LOAD_LOWER_ACTION_TEXT = "Load\nLower Jaw";
const QString LoadingToolBar::ARCHES_ALIGNMENT_ACTION_TEXT = "Arches\nAlignment";

LoadingToolBar::LoadingToolBar(QWidget* parent)
    : QToolBar("Loading Bar", parent)
{
    setMovable(false);
    setIconSize(QSize(28, 28));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    loadUpperAction = new QAction(QIcon(":/DentrixUI/Icons/load-upper.svg"), LOAD_UPPER_ACTION_TEXT, this);

    loadLowerAction = new QAction(QIcon(":/DentrixUI/Icons/load-lower.png"), LOAD_LOWER_ACTION_TEXT, this);

    alignJawsAction = new QAction(QIcon(":/DentrixUI/Icons/alignment.svg"), ARCHES_ALIGNMENT_ACTION_TEXT, this);
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
