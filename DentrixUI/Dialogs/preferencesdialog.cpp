#include "preferencesdialog.h"

#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QMessageBox>
#include <QVBoxLayout>

QString styleSheetstring = R"(
	QLabel {
		color: #222;
		font-size: 14px;
		padding: 4px;
	}

	QCheckBox {
		color: #222;
		font-size: 14px;
		spacing: 10px;
		padding: 4px;
	}

	QSlider::groove:horizontal {
		height: 6px;
		background: #ccc;
		border-radius: 3px;
	}

	QSlider::handle:horizontal {
		background: #007acc;
		border: 1px solid #005f99;
		width: 14px;
		margin: -4px 0;
		border-radius: 7px;
	}

	QSlider::sub-page:horizontal {
		background: #a0c8e8;
		border-radius: 3px;
	}

	QPushButton {
		background-color: #e0e0e0;
		border: 1px solid #aaa;
		border-radius: 4px;
		padding: 6px 12px;
		font-size: 13px;
	}

	QPushButton:hover {
		background-color: #d6d6d6;
	}

	QPushButton:pressed {
		background-color: #c0c0c0;
	}
)";

PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent)
{
    QHBoxLayout* boundingBoxLayout = new QHBoxLayout();
    QLabel* boundingBoxLabel = new QLabel("Show Bounding Box: ");
    boundingBoxCheck = new QCheckBox();
    boundingBoxLayout->addWidget(boundingBoxLabel);
    boundingBoxLayout->addWidget(boundingBoxCheck);

    QHBoxLayout* wireframeLayout = new QHBoxLayout;
    QLabel* wireframeLabel = new QLabel("Show Wireframe: ");
    wireframeCheck = new QCheckBox();
    wireframeLayout->addWidget(wireframeLabel);
    wireframeLayout->addWidget(wireframeCheck);

    QHBoxLayout* sensitivityLayout = new QHBoxLayout;
    cameraSensitivitySlider = new QSlider(Qt::Horizontal);
    cameraSensitivitySlider->setRange(1, 100);
    cameraSensitivitySlider->setValue(50);
    sensitivityLabel = new QLabel("Camera Sensitivity: 50");
    sensitivityLabel->show();
    connect(cameraSensitivitySlider, &QSlider::valueChanged, this, &PreferencesDialog::updateSensitivityLabel);
    sensitivityLayout->addWidget(sensitivityLabel);
    sensitivityLayout->addWidget(cameraSensitivitySlider);

    saveButton = new QPushButton("Save");
    resetButton = new QPushButton("Reset");
    cancelButton = new QPushButton("Cancel");

    connect(saveButton, &QPushButton::clicked, this, &PreferencesDialog::onSaveClicked);
    connect(resetButton, &QPushButton::clicked, this, &PreferencesDialog::onResetClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(boundingBoxLayout);
    mainLayout->addLayout(wireframeLayout);
    mainLayout->addLayout(sensitivityLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    adjustSize();
    setFixedHeight(200);
    setFixedWidth(400);
    setWindowTitle("Preferences");
    setStyleSheet(styleSheetstring);

    loadSettings();
}

void PreferencesDialog::updateSensitivityLabel()
{
    int value = cameraSensitivitySlider->value();
    sensitivityLabel->setText(QString("Camera Sensitivity: %1%").arg(value));
}

QString PreferencesDialog::settingsFilePath() const
{
    return settingsPath;
}

void PreferencesDialog::loadSettings()
{
    QFile file(settingsFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    boundingBoxCheck->setChecked(obj["show_bounding_box"].toBool());
    wireframeCheck->setChecked(obj["show_wireframes"].toBool());
    cameraSensitivitySlider->setValue(obj["camera_sensitivity"].toInt(50));
    updateSensitivityLabel();
}

void PreferencesDialog::saveSettings()
{
    QJsonObject obj;
    obj["show_bounding_box"] = boundingBoxCheck->isChecked();
    obj["show_wireframes"] = wireframeCheck->isChecked();
    obj["camera_sensitivity"] = cameraSensitivitySlider->value();

    QJsonDocument doc(obj);
    QFile file(settingsFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Failed to save preferences.");
        return;
    }
    file.write(doc.toJson());
}

Preferences PreferencesDialog::getPreferences()
{
    Preferences preferences;

    QFile file(settingsPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return preferences;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    preferences.showBoundingBox = (obj["show_bounding_box"].toBool());
    preferences.showWireframes = (obj["show_wireframes"].toBool());
    preferences.cameraSensitivity = (obj["camera_sensitivity"].toInt(50));

    return preferences;
}

void PreferencesDialog::onSaveClicked()
{
    saveSettings();
    accept();
}

void PreferencesDialog::onResetClicked()
{
    boundingBoxCheck->setChecked(false);
    wireframeCheck->setChecked(false);
    cameraSensitivitySlider->setValue(50);
    updateSensitivityLabel();
}
