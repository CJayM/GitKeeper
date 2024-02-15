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
    QByteArray splitter_2_state;
    QByteArray splitter_4_state;
    QByteArray splitter_state;
    QString gitPath;
};

