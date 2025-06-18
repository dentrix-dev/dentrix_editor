#include "toolbarwidget.h"

#include <QActionGroup>
#include <QWidgetAction>

const QString ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT = "Uniform Scale";
const QString ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT = "Directional Scale";
const QString ToolBarWidget::FREE_DEFORM_ACTION_TEXT = "Free Deformation";
const QString ToolBarWidget::ROTATE_ACTION_TEXT = "Rotate";
const QString ToolBarWidget::MOVE_TOOTH_ACTION_TEXT = "Move Tooth";

ToolBarWidget::ToolBarWidget() : QToolBar("Side Toolbar")
{
    setMovable(false);
    // Create Transformation Actions Group
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    // Create actions (buttons) to add to the toolbar
    QAction *uniformScaleAction = new QAction(ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT);
    uniformScaleAction->setCheckable(true);
    uniformScaleAction->setIcon(QIcon(":/DentrixUI/Icons/uniform-scale.svg"));
    uniformScaleAction->setToolTip(ToolBarWidget::UNIFORM_SCALE_ACTION_TEXT);
    actionGroup->addAction(uniformScaleAction);

    QAction *directionalScaleAction = new QAction(ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT);
    directionalScaleAction->setCheckable(true);
    directionalScaleAction->setIcon(QIcon(":/DentrixUI/Icons/directional-scale.svg"));
    directionalScaleAction->setToolTip(ToolBarWidget::DIRECTIONAL_SCALE_ACTION_TEXT);
    actionGroup->addAction(directionalScaleAction);

    QAction *freeDeformAction = new QAction(ToolBarWidget::FREE_DEFORM_ACTION_TEXT);
    freeDeformAction->setCheckable(true);
    freeDeformAction->setIcon(QIcon(":/DentrixUI/Icons/free-deform.svg"));
    freeDeformAction->setToolTip(ToolBarWidget::FREE_DEFORM_ACTION_TEXT);
    actionGroup->addAction(freeDeformAction);

    QAction *rotateAction = new QAction(ToolBarWidget::ROTATE_ACTION_TEXT);
    rotateAction->setCheckable(true);
    rotateAction->setIcon(QIcon());
    freeDeformAction->setToolTip(ToolBarWidget::ROTATE_ACTION_TEXT);
    actionGroup->addAction(rotateAction);

    QAction *moveToothAction = new QAction(ToolBarWidget::MOVE_TOOTH_ACTION_TEXT);
    moveToothAction->setCheckable(true);
    moveToothAction->setIcon(QIcon());
    moveToothAction->setToolTip(ToolBarWidget::MOVE_TOOTH_ACTION_TEXT);
    actionGroup->addAction(moveToothAction);

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
    this->addSeparator();
    this->addAction(spacerAction);
    this->addAction(editAction);
    this->setIconSize(QSize(24, 24));
    this->setToolButtonStyle(Qt::ToolButtonIconOnly);

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
