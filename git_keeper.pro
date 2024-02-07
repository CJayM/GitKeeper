QT       += core gui widgets


CONFIG += c++14


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
