QT       += core gui widgets


CONFIG += c++14


SOURCES += \
    app_settings.cpp \
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
