#include "app_settings.h"

#include <QSettings>

void AppSettings::load()
{
    QSettings settings("kb23", "GitKeeper");
    geometry = settings.value("geometry").toByteArray();
    gitPath = settings.value("gitPath", "C:\\Program Files\\Git\\cmd\\git.exe").toString();
}

void AppSettings::save() const
{
    QSettings settings("kb23", "GitKeeper");
    settings.setValue("geometry", geometry);
    settings.setValue("gitPath", gitPath);
}
