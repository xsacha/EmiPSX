QT       += core gui opengl multimedia declarative

TARGET = EmiPSX
TEMPLATE = app

CONFIG += mobility
MOBILITY += systeminfo

#CONFIG += JIT

symbian {
    TARGET.UID3 = 0xA0016204
    TARGET.EPOCSTACKSIZE = 0x20000
    TARGET.EPOCHEAPSIZE = 0x800000 0x1600000
    INCLUDEPATH += /epoc32/include/mmf/server
    LIBS += -lmmfdevsound #required for CMMFDevSound
    QMAKE_LFLAGS += ../port/sdl/div.o ../gpu/gpu_unai/gpu_arm.o ../recompiler/arm/run.o
}

INCLUDEPATH += ../gte/gte_new \
            ../spu/spu_franxis \
            ../gpu/gpu_unai \
            ../port/sdl \
            ../
SOURCES +=\
    virtualkey.cpp \
    emuinterface.cpp \
    mainwindow.cpp \
    main.cpp \
    ../spu/spu_franxis/spu.cpp \
    ../*.cpp \
    ../gte/gte_new/gte.cpp

HEADERS += mainwindow.h \
    virtualkey.h \
    emuinterface.h \
    ../*.h \
    ../gpu/gpu_unai/*.h \
    ../spu/spu_franxis/*.h \
    ../gte/gte_new/*.h \
    ../port/sdl/port.h

JIT {
    SOURCES += ../recompiler/arm/recompiler.cpp
    HEADERS += ../recompiler/arm/*.h
}
else {
    SOURCES += ../interpreter/interpreter_new/psxinterpreter.cpp
}

DEFINES += XA_HACK BIOS_FILE=\"scph1001.bin\" MCD1_FILE=\"mcd001.mcr\" \
    MCD2_FILE=\"mcd002.mcr\" INLINE="__inline"  gpu_unai spu_franxis \
    gte_new MAEMO_CHANGES TIME_IN_MSEC  ARM_ARCH WITH_HLE
JIT {
    DEFINES += DYNAREC interpreter_none PSXREC STD_PSXREC
}
else {
    DEFINES += interpreter_new
}

QMAKE_CXXFLAGS += -O3 -marm -ffast-math -mstructure-size-boundary=32\
-funroll-loops -finline -finline-functions \
-fno-common -fno-builtin

RESOURCES += EmiPSX.qrc
LIBS +=  -lEmiPSXlib.lib

OTHER_FILES += \
    ../gpu/gpu_unai/gpu_arm.s \
    data/*.qml \
    data/UI/*.qml \
    ../recompiler/arm/arm.s \
    ../port/sdl/div.S \
