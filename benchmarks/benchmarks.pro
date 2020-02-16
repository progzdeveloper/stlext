TEMPLATE = app

CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
TARGET = stlext-benchmarks
DESTDIR = ../bin
DEPENDPATH += ../
INCLUDEPATH += ../
OBJECTS_DIR = ../build/gbenchmarks/obj

LIBS += -lbenchmark -lbenchmark_main -lpthread #-lasan

QMAKE_CXXFLAGS += -O3 -m64 -mtune=native -march=native #-g
QMAKE_LFLAGS = -Wl,-O3 #-fsanitize=address,leak
#QMAKE_LFLAGS = -fsanitize=address,leak

SOURCES += \
    bm_bitvector.cpp \
    bm_counting_sort.cpp \
    bm_main.cpp \
    bm_packed_hashtbl.cpp \
    bm_stringset.cpp


