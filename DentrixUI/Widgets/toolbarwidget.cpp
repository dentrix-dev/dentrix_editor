#include "toolbarwidget.h"

#include <QActionGroup>
#include <QWidgetAction>

const QString ToolBarWidget::ActionTexts::UNIFORM_SCALE_ACTION_TEXT = "Uniform\nScale";
const QString ToolBarWidget::ActionTexts::DIRECTIONAL_SCALE_ACTION_TEXT = "Directional\nScale";
const QString ToolBarWidget::ActionTexts::FREE_DEFORM_ACTION_TEXT = "Free\nDeform";
const QString ToolBarWidget::ActionTexts::ROTATE_ACTION_TEXT = "Rotate";
const QString ToolBarWidget::ActionTexts::MOVE_TOOTH_ACTION_TEXT = "Move\nTooth";
const QString ToolBarWidget::ActionTexts::MOVE_JAW_ACTION_TEXT = "Move\nJaw";

ToolBarWidget::ToolBarWidget() : QToolBar("Side Toolbar")
{
    setMovable(false);
    // Create Transformation Actions Group
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    // Create actions (buttons) to add to the toolbar
    QAction *uniformScaleAction =
        new QAction(QIcon(":/DentrixUI/Icons/uniform-scale.svg"), ActionTexts::UNIFORM_SCALE_ACTION_TEXT, this);
    uniformScaleAction->setCheckable(true);
    actionGroup->addAction(uniformScaleAction);

    QAction *directionalScaleAction =
        new QAction(QIcon(":/DentrixUI/Icons/directional-scale.svg"), ActionTexts::DIRECTIONAL_SCALE_ACTION_TEXT, this);
    directionalScaleAction->setCheckable(true);
    actionGroup->addAction(directionalScaleAction);

    QAction *freeDeformAction =
        new QAction(QIcon(":/DentrixUI/Icons/free-deform.svg"), ActionTexts::FREE_DEFORM_ACTION_TEXT, this);
    freeDeformAction->setCheckable(true);
    actionGroup->addAction(freeDeformAction);

    QAction *rotateAction = new QAction(QIcon(":/DentrixUI/Icons/rotate.svg"), ActionTexts::ROTATE_ACTION_TEXT, this);
    rotateAction->setCheckable(true);
    actionGroup->addAction(rotateAction);

    QAction *moveToothAction =
        new QAction(QIcon(":/DentrixUI/Icons/move.svg"), ActionTexts::MOVE_TOOTH_ACTION_TEXT, this);
    moveToothAction->setCheckable(true);
    actionGroup->addAction(moveToothAction);

    QAction *moveJawAction = new QAction(QIcon(":/DentrixUI/Icons/move.svg"), ActionTexts::MOVE_JAW_ACTION_TEXT, this);
    moveJawAction->setCheckable(true);
    actionGroup->addAction(moveJawAction);

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
    this->addAction(uniformScaleAction);
    this->addAction(directionalScaleAction);
    this->addAction(freeDeformAction);
    this->addAction(rotateAction);
    this->addAction(moveToothAction);
    this->addAction(moveJawAction);
    this->addSeparator();
    this->addAction(spacerAction);
    this->addAction(editAction);
    this->setIconSize(QSize(32, 32));
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
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

void ToolBarWidget::set_edit_button_mode(ButtonMode status)
{
    if (status == ButtonMode::Edit_mode) {
        editButton->setText("Save");
        editButton->setToolTip("Save and return to main scene");
    } else {
        editButton->setText("Edit");
        editButton->setToolTip("Click to enter edit mode");
    }
}

void ToolBarWidget::set_edit_button_active(bool active)
{
    if (active)
        editButton->setStyleSheet(editButtonActiveStyleSheet);
    else
        editButton->setStyleSheet(editButtonInactiveStyleSheet);
}
