QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    domain/git.h \
    domain/git_file.h \
    domain/git_repository.h \
    diff.h

SOURCES += \
    domain/git.cpp \
    domain/git_file.cpp \
    domain/git_repository.cpp \
    diff.cpp
