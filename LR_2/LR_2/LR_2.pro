TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpserver.cpp \
    log.cpp \
    mytcpserver.cpp \
    tcpclient.cpp \
    mytcpclient.cpp

HEADERS += \
    server.h \
    tcpserver.h \
    socketapi.h \
    log.h \
    connect.h \
    mytcpserver.h \
    socket.h \
    client.h \
    tcpclient.h \
    mytcpclient.h


LIBS += -pthread
