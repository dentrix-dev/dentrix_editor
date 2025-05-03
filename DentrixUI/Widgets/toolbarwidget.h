#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include <QPushButton>
#include <QToolBar>

class ToolBarWidget : public QToolBar
{
	QActionGroup *actionGroup;
	QPushButton *editButton;

	// should probably be static
	const QString editButtonInactiveStyleSheet =
	    "background-color: gray; color: white; padding: 5px;";
	const QString editButtonActiveStyleSheet =
	    "background-color: #3498db; color: white; padding: 5px;";

public:
	ToolBarWidget();

	QActionGroup *getActionGroup() const;

	static const QString UNIFORM_SCALE_ACTION_TEXT;
	static const QString DIRECTIONAL_SCALE_ACTION_TEXT;
	static const QString FREE_DEFORM_ACTION_TEXT;

	QPushButton *getEditButton();
	QString getEditButtonInactiveStyleSheet() const;
	QString getEditButtonActiveStyleSheet() const;
};

#endif  // TOOLBARWIDGET_H
