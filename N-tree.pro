TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        cmdargparser.cpp \
        main.cpp \
        ntree.cpp \
        ntreeserializer.cpp \
        utility.cpp \

HEADERS += \
    cmdargparser.h \
    ntree.h \
    ntreeserializer.h \
    utility.h \
