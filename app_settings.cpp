#include "app_settings.h"

#include <QSettings>

void AppSettings::load()
{
    QSettings settings("kb23", "GitKeeper");
    geometry = settings.value("geometry").toByteArray();
    gitPath = settings.value("gitPath", "C:\\Program Files\\Git\\cmd\\git.exe").toString();

    splitter_2_state = settings.value("splitter_2_state").toByteArray();
    splitter_4_state = settings.value("splitter_4_state").toByteArray();
    splitter_state = settings.value("splitter_state").toByteArray();
    splitter_3_state = settings.value("splitter_3_state").toByteArray();
}

void AppSettings::save() const
{
    QSettings settings("kb23", "GitKeeper");
    settings.setValue("geometry", geometry);
    settings.setValue("gitPath", gitPath);

    settings.setValue("splitter_2_state", splitter_2_state);
    settings.setValue("splitter_4_state", splitter_4_state);
    settings.setValue("splitter_state", splitter_state);
    settings.setValue("splitter_3_state", splitter_3_state);
}
