QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatlistdelegate.cpp \
    chatlistview.cpp \
    chatwidget.cpp \
    commonutils.cpp \
    friendlistdelegate.cpp \
    friendlistview.cpp \
    jsonmanager.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    message.cpp \
    messageprocessor.cpp \
    msgsocket.cpp \
    signupdialog.cpp \
    user.cpp

HEADERS += \
    chatlistdelegate.h \
    chatlistview.h \
    chatwidget.h \
    commonutils.h \
    friendlistdelegate.h \
    friendlistview.h \
    jsonmanager.h \
    logindialog.h \
    mainwindow.h \
    message.h \
    messageprocessor.h \
    msgsocket.h \
    signupdialog.h \
    user.h

FORMS += \
    chatwidget.ui \
    logindialog.ui \
    mainwindow.ui \
    signupdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
