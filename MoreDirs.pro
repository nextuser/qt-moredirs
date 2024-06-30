QT       += core gui core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
ICON = moredirs.icns
RC_ICONS = moredirs.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bookmarkMgr.cpp \
    fileclipboard.cpp \
    fileutil.cpp \
    thistory.cpp \
    dirform.cpp \
    main.cpp \
    mainwindow.cpp \
    tsubwindow.cpp


HEADERS += \
    bookmarkMgr.h \
    fileclipboard.h \
    fileutil.h \
    thistory.h \
    dirform.h \
    mainwindow.h \
    tsubwindow.h

FORMS += \
    dirform.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
