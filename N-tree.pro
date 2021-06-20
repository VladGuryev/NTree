TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += qt
QT += core

SOURCES += \
        main.cpp \
        ntree.cpp \
        ntreeserializer.cpp \
 \  #      args.cpp
    utility.cpp

HEADERS += \
    ntree.h \
    ntreeserializer.h \
 \  #  args.h
    utility.h
