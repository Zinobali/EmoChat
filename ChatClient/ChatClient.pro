QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = app.ico
DESTDIR = ./bin

SOURCES += \
    adduseritem.cpp \
    bubbleframe.cpp \
    chatdialog.cpp \
    chatitembase.cpp \
    chatpage.cpp \
    chatuserdelegate.cpp \
    chatuserlist.cpp \
    chatuserlistmodel.cpp \
    chatview.cpp \
    clickedbtn.cpp \
    clickedlabel.cpp \
    global.cpp \
    httpmgr.cpp \
    listitembase.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    msgtextedit.cpp \
    picturebubble.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    searchlist.cpp \
    statewidget.cpp \
    tcpmgr.cpp \
    textbubble.cpp \
    timerbtn.cpp \
    usermgr.cpp

HEADERS += \
    adduseritem.h \
    bubbleframe.h \
    chatdialog.h \
    chatitembase.h \
    chatpage.h \
    chatuserdelegate.h \
    chatuserlist.h \
    chatuserlistmodel.h \
    chatview.h \
    clickedbtn.h \
    clickedlabel.h \
    global.h \
    httpmgr.h \
    listitembase.h \
    logindialog.h \
    mainwindow.h \
    msgtextedit.h \
    picturebubble.h \
    registerdialog.h \
    resetdialog.h \
    searchlist.h \
    singleton.h \
    statewidget.h \
    tcpmgr.h \
    textbubble.h \
    timerbtn.h \
    usermgr.h

FORMS += \
    adduseritem.ui \
    chatdialog.ui \
    chatpage.ui \
    chatview.ui \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    resetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

CONFIG(release, debug | release)
{
    TargetConfig = $${PWD}/config.ini
    #将输入目录中的"/"替换为"\"
    TargetConfig = $$replace(TargetConfig, /, \\)
    #将输出目录中的"/"替换为"\"
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    //执行copy命令
    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
}

msvc:QMAKE_CXXFLAGS +=  -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS +=  -source-charset:utf-8
