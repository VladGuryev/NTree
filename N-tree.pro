TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        ntree.cpp \
        ntreeserializer.cpp \
        args.cpp

HEADERS += \
    ntree.h \
    ntreeserializer.h \
    args.h
