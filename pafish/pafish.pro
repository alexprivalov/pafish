TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

include($$PWD/libpafish.pri)

SOURCES += \
        main.c


CONFIG += static_runtime

win32-msvc* {
    QMAKE_CFLAGS=  -Oy- -Gy -Gw -GL -MT
    QMAKE_CFLAGS_DEBUG = -Zi -MTd
    QMAKE_LFLAGS = /NOLOGO /DYNAMICBASE /NXCOMPAT /LTCG
    QMAKE_LFLAGS_RELEASE = /NOLOGO /DYNAMICBASE /NXCOMPAT /LTCG /OPT:REF /OPT:ICF=5 /MANIFEST:EMBED /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT /SUBSYSTEM:CONSOLE  #/DEBUG /OPT:REF /OPT:ICF=5 /MANIFEST:EMBED /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT
    QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO = /DEBUG /OPT:REF /OPT:ICF=5 /INCREMENTAL:NO
}
#/NOLOGO /DYNAMICBASE /NXCOMPAT /LTCG /OPT:REF /OPT:ICF=5 /MANIFEST:EMBED /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT
#
#/NOLOGO /DYNAMICBASE /NXCOMPAT /LTCG /OPT:REF /OPT:ICF=5 /MANIFEST:EMBED /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT /SUBSYSTEM:CONSOLE
