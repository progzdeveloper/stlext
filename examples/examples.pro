TEMPLATE = app

CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
TARGET = stlext-examples
DESTDIR = ../bin
DEPENDPATH += ../
INCLUDEPATH += ../
OBJECTS_DIR = ../build/examples/obj

LIBS += -lpthread

QMAKE_CXXFLAGS += -O1 -m64 -msse4.2

HEADERS += \
    algorithm/majelem_sample.h \
    cui/cui_samples.h \
    iostreams/byteview.h \
    iostreams/codecs.h \
    iostreams/getline.h \
    iostreams/iomanips.h \
    iostreams/utilities.h \
    point2d.h \
    record.h \
    rt.h

SOURCES += \
    main.cpp


