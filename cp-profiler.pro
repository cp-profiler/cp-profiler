TEMPLATE = app

TARGET = cp-profiler

QT += widgets network

CONFIG += c++11

include(cp-profiler.pri)
SOURCES += $$PWD/src/main_cpprofiler.cpp
