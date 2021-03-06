#!/bin/bash

curdir=${PWD##*/}

echo "TEMPLATE = app"
echo ""
echo "CONFIG += console c++1z"
echo "CONFIG -= app_bundle"
echo "CONFIG -= qt"
echo "TARGET = stlext-benchmarks"
echo "DESTDIR = ../bin"
echo "DEPENDPATH += ../"
echo "INCLUDEPATH += ../"
echo "OBJECTS_DIR = ../build/gbenchmarks/obj"
echo ""
echo "LIBS += -lbenchmark -lbenchmark_main -lpthread"
echo ""
echo "QMAKE_CXXFLAGS += -O3 -m64 -msse4.2 -mtune=corei7"
echo "QMAKE_LFLAGS = -Wl,-O2"
echo ""


cd ..
headers=( $(find $curdir -type f -name *.h* ! -name *.c* ! -name *.vcxproj* ! -name *.dox  | sort) )
if [ ${#headers[@]} -gt 0 ]
then
   echo "HEADERS += \\"
   i=0
   while [ $i -lt ${#headers[@]} ]
   do
       j=$i;
       ((j++))
       if [ $j -lt ${#headers[@]} ]
       then
          echo "   " ${headers[$i]#*/} "\\"
       else
          echo "   " ${headers[$i]#*/}
       fi
       ((i++))
   done
fi

echo ""


sources=( $(find $curdir -type f -name *.c* ! -name *.h* ! -name *.vcxproj* ! -name *.dox | sort) )
if [ ${#sources[@]} -gt 0 ]
then
   echo "SOURCES += \\"
   i=0
   while [ $i -lt ${#sources[@]} ]
   do
       j=$i;
       ((j++))
       if [ $j -lt ${#sources[@]} ]
       then
          echo "   " ${sources[$i]#*/} "\\"
       else
          echo "   " ${sources[$i]#*/}
       fi
       ((i++))
   done
fi

echo ""
echo ""

cd $curdir

