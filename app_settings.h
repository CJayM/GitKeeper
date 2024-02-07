#include <QByteArray>
#include <QString>

#pragma once

class AppSettings
{
public:
    AppSettings() = default;
    void load();
    void save() const;

    QByteArray geometry;
    QString gitPath;
};

