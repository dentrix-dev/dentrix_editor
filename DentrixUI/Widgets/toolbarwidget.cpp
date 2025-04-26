#include "toolbarwidget.h"
#include <QActionGroup>
#include <QWidgetAction>

const QString ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT = "Uniform Scale";
const QString ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT = "Directional Scale";
const QString ToolBarWidget::FREE_DEFORM_ACTION_TEXT  = "Free Deformation";

ToolBarWidget::ToolBarWidget():
    QToolBar("Side Toolbar")
{
    //Create Transformation Actions Group
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    // Create actions (buttons) to add to the toolbar
    QAction *uniformTranslateAction = new QAction(ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT);
    uniformTranslateAction->setCheckable(true);
    actionGroup->addAction(uniformTranslateAction);
    QAction *saveAction = new QAction(ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT);
    saveAction->setCheckable(true);
    actionGroup->addAction(saveAction);
    QAction *exitAction = new QAction(ToolBarWidget::FREE_DEFORM_ACTION_TEXT);
    exitAction->setCheckable(true);
    actionGroup->addAction(exitAction);

    // Add a spacer widget to push following widgets to the bottom
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidgetAction *spacerAction = new QWidgetAction(actionGroup);
    spacerAction->setDefaultWidget(spacer);
    // Create a styled blue "Edit" button
    editButton = new QPushButton("Edit");
    editButton->setStyleSheet(editButtonInactiveStyleSheet);
    // Wrap it in a QWidgetAction so it can go in the QToolBar
    QWidgetAction *editAction = new QWidgetAction(actionGroup);
    editAction->setDefaultWidget(editButton);

    // Add the actions to the toolbar
    this->addAction(uniformTranslateAction);
    this->addAction(saveAction);
    this->addAction(exitAction);
    this->addSeparator();
    this->addAction(spacerAction);
    this->addAction(editAction);

    // Add toolbar to the left side

}

QActionGroup *ToolBarWidget::getActionGroup() const
{
    return actionGroup;
}

QPushButton *ToolBarWidget::getEditButton()
{
    return editButton;
}

QString ToolBarWidget::getEditButtonInactiveStyleSheet() const
{
    return editButtonInactiveStyleSheet;
}

QString ToolBarWidget::getEditButtonActiveStyleSheet() const
{
    return editButtonActiveStyleSheet;
}


