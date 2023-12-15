QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Container.cpp \
    LoadUnload.cpp \
    Move.cpp \
    Node.cpp \
    Operation.cpp \
    Search.cpp \
    Search2.cpp \
    Slot.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Container.h \
    LoadUnload.h \
    Move.h \
    Node.h \
    Operation.h \
    Search.h \
    Search2.h \
    Slot.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
