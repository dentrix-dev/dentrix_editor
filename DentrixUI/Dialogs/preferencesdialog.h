#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

struct Preferences {
    bool showBoundingBox;
    bool showWireframes;
    int cameraSensitivity;
};

const QString settingsPath = "preferences.json";

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(QWidget *parent = nullptr);
    void loadSettings();
    void saveSettings();

    static Preferences getPreferences();

private slots:
    void onSaveClicked();
    void onResetClicked();

private:
    QCheckBox *boundingBoxCheck;
    QCheckBox *wireframeCheck;
    QSlider *cameraSensitivitySlider;
    QLabel *sensitivityLabel;

    QPushButton *saveButton;
    QPushButton *resetButton;
    QPushButton *cancelButton;

    void updateSensitivityLabel();
    QString settingsFilePath() const;
};

#endif  // PREFERENCESDIALOG_H
