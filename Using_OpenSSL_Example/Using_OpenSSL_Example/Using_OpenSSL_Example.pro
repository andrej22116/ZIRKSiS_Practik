TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


PKGCONFIG += openssl

LIBS += -lcrypto
LIBS += -lssl
