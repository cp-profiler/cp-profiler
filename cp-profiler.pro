TEMPLATE = app

TARGET = cp-profiler

QT += widgets network

CONFIG += c++11

SOURCES += $$PWD/src/main_cpprofiler.cpp

include(cp-profiler.pri)