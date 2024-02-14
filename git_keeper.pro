QT       += core gui widgets

TARGET = GitKeeper
TEMPLATE = app

CONFIG += c++14
CONFIG += separate_debug_info

VERSION = 0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_NAME=\\\"GitKeeper\\\"
DEFINES += APP_DOMAIN=\\\"jay\\\"

RC_ICONS = resources/app_icon_64.ico


SOURCES += \
    app_settings.cpp \
    code_editor.cpp \
    diff.cpp \
    domain/git.cpp \
    domain/git_file.cpp \
    domain/git_repository.cpp \
    highlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    models/git_files_model.cpp \
    settings_dialog.cpp

HEADERS += \
    app_settings.h \
    code_editor.h \
    diff.h \
    domain/git.h \
    domain/git_file.h \
    domain/git_repository.h \
    highlighter.h \
    mainwindow.h \
    models/git_files_model.h \
    settings_dialog.h

FORMS += \
    mainwindow.ui \
    settings_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	todo.md

RESOURCES += \
    app_resorces.qrc
