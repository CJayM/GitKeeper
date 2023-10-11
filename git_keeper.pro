QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    domain/git.cpp \
    domain/git_file.cpp \
    domain/git_repository.cpp \
    main.cpp \
    mainwindow.cpp \
    models/git_files_model.cpp

HEADERS += \
    domain/git.h \
    domain/git_file.h \
    domain/git_repository.h \
    mainwindow.h \
    models/git_files_model.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	todo.md
