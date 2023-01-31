DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += \
    bochs.c \
    common.c \
    cpu.c \
    debuggers.c \
    gensandbox.c \
    hooks.c \
    qemu.c \
    rtt.c \
    sandboxie.c \
    utils.c \
    vbox.c \
    vmware.c \
    wine.c

HEADERS += \
    bochs.h \
    common.h \
    config.h \
    cpu.h \
    debuggers.h \
    gensandbox.h \
    hooks.h \
    qemu.h \
    rtt.h \
    sandboxie.h \
    types.h \
    utils.h \
    vbox.h \
    vmware.h \
    wine.h

LIBS+= -luser32 -lAdvapi32 -lShell32 -lwsock32 -liphlpapi -lsetupapi -lmpr -lole32 -lwbemuuid -loleaut32 -lws2_32
