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
    app_palette.cpp \
    app_settings.cpp \
    widgets/code_editor.cpp \    
    highlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    models/git_files_model.cpp \
    settings_dialog.cpp \
    widgets/line_number_area.cpp

HEADERS += \
    app_palette.h \
    app_settings.h \
    widgets/code_editor.h \    
    highlighter.h \
    mainwindow.h \
    models/git_files_model.h \
    settings_dialog.h \
    widgets/line_number_area.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../git_keeper/release/ -lgit_keeper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../git_keeper/debug/ -lgit_keeper
else:unix: LIBS += -L$$OUT_PWD/../git_keeper/ -lgit_keeper

INCLUDEPATH += $$PWD/../git_keeper
DEPENDPATH += $$PWD/../git_keeper

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../git_keeper/release/libgit_keeper.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../git_keeper/debug/libgit_keeper.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../git_keeper/release/git_keeper.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../git_keeper/debug/git_keeper.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../git_keeper/libgit_keeper.a
