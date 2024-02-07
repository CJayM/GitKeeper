#pragma once

#include "app_settings.h"

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    void saveSettings(AppSettings &settings);
    void loadSettings(const AppSettings &settings);

private slots:
    void onBrowseGitPathClicked();

private:
    Ui::SettingsDialog *ui;
};

