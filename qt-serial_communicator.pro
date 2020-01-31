QT -= gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

INCLUDEPATH += include

SOURCES += \
    src/communicator.cpp \
    src/inbound.cpp \
    src/message.cpp \
    src/outbound.cpp

HEADERS += \
    include/pcd/qt-serial_communicator/communicator.h \
    include/pcd/qt-serial_communicator/message.h \
    include/pcd/qt-serial_communicator/message_status.h \
    include/pcd/qt-serial_communicator/utility/inbound.h \
    include/pcd/qt-serial_communicator/utility/outbound.h

target.path = /usr/local/lib
headers.path = /usr/local/include
headers.files = include/*
INSTALLS += target headers
