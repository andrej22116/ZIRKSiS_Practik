TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpserver.cpp \
    log.cpp \
    mytcpserver.cpp \
    tcpclient.cpp \
    mytcpclient.cpp \
    trololo.cpp \
    tlsconnect.cpp \
    tlsserver.cpp \
    sslclasses.cpp \
    mytlsserver.cpp \
    tlsclient.cpp \
    mytlsclient.cpp

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
    mytcpclient.h \
    tlsconnect.h \
    sslclasses.h \
    tlsserver.h \
    mytlsserver.h \
    tlsclient.h \
    mytlsclient.h


LIBS += -pthread



PKGCONFIG += openssl

LIBS += -lcrypto
LIBS += -lssl
