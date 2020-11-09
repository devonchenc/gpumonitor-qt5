#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
    SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() {}

private:
	void initUI();

	void loadSettings();

private slots:
	void browseButtonClicked();

	void acceptButtonClicked();

private:
    QLineEdit* _pathEdit;
};

#endif // SETTINGSDIALOG_H
