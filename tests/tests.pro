TEMPLATE = app

CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
TARGET = stlext-tests
DESTDIR = ../bin
DEPENDPATH += ../ ../catch
INCLUDEPATH += ../ ../catch
OBJECTS_DIR = ../build/tests/obj

LIBS += -lpthread -lasan

#QMAKE_CXXFLAGS += -O1 -m64 -msse4.2 -g -fno-omit-frame-pointer -Wall -Wextra
QMAKE_CXXFLAGS += -fno-omit-frame-pointer -Wall -Wextra
QMAKE_LFLAGS += -fsanitize=address


SOURCES += \
    algorithm/experimental.cpp \
    algorithm/sorting.cpp \
    bitvector/bitvector.cpp \
    compability/c++11_algo.cpp \
    compability/c++14_algo.cpp \
    compability/c++17_algo.cpp \
    compability/c++20_algo.cpp \
    compact/string.cpp \
    compact/vector.cpp \
    compact/wstring.cpp \
    components/class_factory.cpp \
    containers/packed_hashtbl.cpp \
    containers/stringset.cpp \
    functional/predicates.cpp \
    iostreams/base16.cpp \
    iostreams/base64.cpp \
    iostreams/binrep.cpp \
    iostreams/getline.cpp \
    iostreams/ostream_joiner.cpp \
    iostreams/ratio.cpp \
    iterator/iterators.cpp \
    main.cpp \
    containers/lru_cache.cpp \
    containers/span.cpp \
    range/interval.cpp


