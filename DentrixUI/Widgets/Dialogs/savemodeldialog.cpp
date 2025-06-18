#include "savemodeldialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SaveModelDialog::SaveModelDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Save File");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(16);

    // Path input section
    auto *fileLayout = new QHBoxLayout();
    auto *pathLabel = new QLabel("File path:", this);
    pathLineEdit = new QLineEdit(this);
    browseButton = new QPushButton("Browse...", this);

    // Ensure the label doesn't stretch
    pathLabel->setFixedWidth(70);

    connect(browseButton, &QPushButton::clicked, this, &SaveModelDialog::browse);

    fileLayout->addWidget(pathLabel);
    fileLayout->addWidget(pathLineEdit, 1);  // Stretch to fill space
    fileLayout->addWidget(browseButton);

    mainLayout->addLayout(fileLayout);

    // Spacer between input and buttons
    mainLayout->addSpacing(10);

    // Button section
    auto *buttonLayout = new QHBoxLayout();
    saveButton = new QPushButton("Save", this);
    cancelButton = new QPushButton("Cancel", this);

    connect(saveButton, &QPushButton::clicked, this, &SaveModelDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &SaveModelDialog::reject);

    // Add buttons aligned to right
    buttonLayout->addStretch();  // Push buttons to the right
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Optional: Set a fixed width for better appearance on small windows
    setMinimumWidth(400);
}

QString SaveModelDialog::getPath() const
{
    return pathLineEdit->text();
}

void SaveModelDialog::browse()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Select File", "", "All Files (*)");
    if (!fileName.isEmpty()) {
        pathLineEdit->setText(fileName);
    }
}

void SaveModelDialog::accept()
{
    if (!pathLineEdit->text().isEmpty()) {
        QDialog::accept();
    }
}
