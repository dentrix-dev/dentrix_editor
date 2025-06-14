#include "loadingbar.h"

#include <QIcon>
#include <iostream>

const QString LoadingBar::LOAD_UPPER_ACTION_TEXT = "Load Upper Jaw";
const QString LoadingBar::LOAD_LOWER_ACTION_TEXT = "Load Lower Jaw";
const QString LoadingBar::ALIGN_JAWS_ACTION_TEXT = "Align Jaws";

LoadingBar::LoadingBar(QWidget* parent)
    : QToolBar("Loading Bar", parent)
{
    std::cout << "Initializing LoadingBar..." << std::endl;
    setMovable(false);
    setIconSize(QSize(24, 24));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    loadUpperAction = new QAction(QIcon(":/DentrixUI/Icons/load-lower.svg"), LOAD_UPPER_ACTION_TEXT, this);
    loadUpperAction->setToolTip(LOAD_UPPER_ACTION_TEXT);

    loadLowerAction = new QAction(QIcon(":/DentrixUI/Icons/load-lower.svg"), LOAD_LOWER_ACTION_TEXT, this);
    loadLowerAction->setToolTip(LOAD_LOWER_ACTION_TEXT);

    alignJawsAction = new QAction(QIcon(":/DentrixUI/Icons/load-lower.svg"), ALIGN_JAWS_ACTION_TEXT, this);
    alignJawsAction->setToolTip(ALIGN_JAWS_ACTION_TEXT);
    alignJawsAction->setEnabled(false);  // initially disabled

    addAction(loadUpperAction);
    addAction(loadLowerAction);
    addSeparator();
    addAction(alignJawsAction);

    connect(loadUpperAction, &QAction::triggered, this, [this]() {
        std::cout << "LoadingBar: loadUpperRequested signal emitted" << std::endl;
        emit loadUpperRequested();
    });
    
    connect(loadLowerAction, &QAction::triggered, this, [this]() {
        std::cout << "LoadingBar: loadLowerRequested signal emitted" << std::endl;
        emit loadLowerRequested();
    });
    
    connect(alignJawsAction, &QAction::triggered, this, [this]() {
        std::cout << "LoadingBar: alignJawsRequested signal emitted" << std::endl;
        emit alignJawsRequested();
    });
    
    std::cout << "LoadingBar initialized successfully" << std::endl;
}

void LoadingBar::setAlignEnabled(bool enabled)
{
    std::cout << "Setting align button enabled: " << (enabled ? "true" : "false") << std::endl;
    alignJawsAction->setEnabled(enabled);
}
