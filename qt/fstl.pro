QT += core gui opengl widgets

TARGET = fstl
TEMPLATE = app

# Bump optimization up to -O3 in release builds
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += \
    ../src/app.cpp\
    ../src/main.cpp\
    ../src/canvas.cpp \
    ../src/mesh.cpp \
    ../src/glmesh.cpp \
    ../src/loader.cpp \
    ../src/window.cpp \
    ../src/backdrop.cpp \
    ../src/globject.cpp \
    ../src/testpattern.cpp \
    ../src/antennadata.cpp \
    ../src/dataprocessor.cpp \
    ../src/glcolormesh.cpp \
    ../src/radpatternloader.cpp

HEADERS  += \
    ../src/app.h\
    ../src/canvas.h \
    ../src/mesh.h \
    ../src/glmesh.h \
    ../src/loader.h \
    ../src/window.h \
    ../src/backdrop.h \
    ../src/globject.h \
    ../src/testpattern.h \
    ../src/antennadata.h \
    ../src/dataprocessor.h \
    ../src/glcolormesh.h \
    ../src/radpatternloader.h

CONFIG += c++11

RESOURCES += \
    qt.qrc \
    ../gl/gl.qrc

macx {
    QMAKE_INFO_PLIST = ../app/Info.plist
    ICON = ../app/fstl.icns
}

win32 {
    RC_FILE = ../exe/fstl.rc
}

linux {
    target.path = /usr/bin
    INSTALLS += target
}

static {
    CONFIG += static
}
