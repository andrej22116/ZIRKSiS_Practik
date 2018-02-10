TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    aesalgorithm.cpp

PKGCONFIG += openssl

LIBS += -lcrypto
LIBS += -lssl

HEADERS += \
    aesalgorithm.h
