QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -gstabs+

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfrienddialog.cpp \
    chatinfomanage.cpp \
    custom/toolboxpow.cpp \
    custom/chatmessage.cpp \
    logindlg.cpp \
    main.cpp \
    net/chatClient.cpp \
    net/chatService.cpp \
    net/netpublic.cpp \
    registdlg.cpp \
    widget.cpp

HEADERS += \
    addfrienddialog.h \
    chatinfomanage.h \
    common.h \
    custom/toolboxpow.h \
    custom/chatmessage.h \
    logindlg.h \
    net/chatClient.hpp \
    net/chatService.hpp \
    net/netpublic.hpp \
    net/public.hpp \
    registdlg.h \
    widget.h \
    json.hpp \

FORMS += \
    addfrienddialog.ui \
    logindlg.ui \
    registdlg.ui \
    widget.ui

LIBS += -L/usr/lib/ -lmuduo_net\
        -L/usr/lib/ -lmuduo_base\
        -lpthread\

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

RC_ICONS = wechat.ico
