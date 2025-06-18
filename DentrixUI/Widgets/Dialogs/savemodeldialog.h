#ifndef SAVEMODELDIALOG_H
#define SAVEMODELDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class SaveModelDialog : public QDialog
{
    Q_OBJECT
public:
    SaveModelDialog(QWidget *parent = nullptr);
    QString getPath() const;
private slots:
    void browse();
    void accept() override;

private:
    QLineEdit *pathLineEdit;
    QPushButton *browseButton;
    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif  // SAVEMODELDIALOG_H
