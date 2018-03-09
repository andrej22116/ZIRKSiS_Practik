TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpserver.cpp \
    log.cpp \
    mytcpserver.cpp

HEADERS += \
    server.h \
    tcpserver.h \
    socketapi.h \
    log.h \
    connect.h \
    mytcpserver.h \
    socket.h


LIBS += -pthread
