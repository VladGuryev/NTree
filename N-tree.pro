TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        cmdargparser.cpp \
        main.cpp \
        ntreeserializer.cpp \
    printtree.cpp \
    typeserializers.cpp \
        utility.cpp \

HEADERS += \
    cmdargparser.h \
    ntree.h \
    ntreeserializer.h \
    printtree.h \
    typeserializers.h \
    utility.h \
