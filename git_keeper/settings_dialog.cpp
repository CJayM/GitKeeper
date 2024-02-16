#include "settings_dialog.h"
#include "ui_settings_dialog.h"

#include <QDebug>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->browseGitPath,
            &QAbstractButton::clicked,
            this,
            &SettingsDialog::onBrowseGitPathClicked);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings(AppSettings &settings)
{
    settings.gitPath = ui->gitPathEdit->text();
}

void SettingsDialog::loadSettings(const AppSettings &settings)
{
    ui->gitPathEdit->setText(settings.gitPath);
}

void SettingsDialog::onBrowseGitPathClicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(("Git binary (git*.exe git)"));
    if (dialog.exec() == false)
        return;

    auto fileNames = dialog.selectedFiles();
    if (fileNames.isEmpty())
        return;

    ui->gitPathEdit->setText(fileNames.first());
}
