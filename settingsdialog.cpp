#include "settingsdialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	initUI();

	setWindowFlag(Qt::Popup);

    setWindowTitle(tr("Settings"));

    resize(500, 100);

	loadSettings();
}

void SettingsDialog::initUI()
{
    _pathEdit = new QLineEdit("");
	QPushButton* browseButton = new QPushButton("...");
	browseButton->setMaximumWidth(30);
    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::browseButtonClicked);

    QGridLayout* grid = new QGridLayout();
    grid->addWidget(new QLabel(tr("NVIDIA-SMI path:")), 0, 0);
    grid->addWidget(_pathEdit, 0, 1, 1, 2);
    grid->addWidget(browseButton, 0, 3);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 2);
    grid->setColumnStretch(2, 1);
    QGroupBox* groupBox = new QGroupBox(tr("Path"));
    groupBox->setLayout(grid);

    QPushButton* acceptButton = new QPushButton(tr("OK"));
    connect(acceptButton, &QPushButton::clicked, this, &SettingsDialog::acceptButtonClicked);
	QPushButton* rejectButton = new QPushButton(tr("Cancel"));
	connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addStretch();
	hLayout->addWidget(acceptButton);
	hLayout->addStretch();
	hLayout->addWidget(rejectButton);
	hLayout->addStretch();

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(groupBox);
	vLayout->addLayout(hLayout);
	setLayout(vLayout);
}

void SettingsDialog::loadSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    QString path = settings.value("Path/NV", "C:/Program Files/NVIDIA Corporation/NVSMI/nvidia-smi.exe").toString();
    _pathEdit->setText(path);
}

void SettingsDialog::browseButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select nvidia-smi.exe"), _pathEdit->text());
	if (filePath.isEmpty() == false)
	{
		_pathEdit->setText(filePath);
	}
}

void SettingsDialog::acceptButtonClicked()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.setValue("Path/NV", _pathEdit->text());

	accept();
}
