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
