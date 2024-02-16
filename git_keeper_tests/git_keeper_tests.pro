QT += testlib
QT += gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += c++14

TEMPLATE = app

OBJECTS_DIR = _temp_build/objs
MOC_DIR = _temp_build/moc

SOURCES +=  \
    main.cpp \
    test_diff_scroll_mapper.cpp



HEADERS += \
    test_diff_scroll_mapper.h

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
