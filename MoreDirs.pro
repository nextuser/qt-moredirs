QT       += core gui core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
ICON = moredirs.icns
RC_ICONS = moredirs.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TRANSLATIONS = moredirs_en.ts \
    moredirs_cn.ts
SOURCES += \
    bookmarkMgr.cpp \
    copyoptions.cpp \
    copyprocessdialog.cpp \
    duplicateddialog.cpp \
    fileclipboard.cpp \
    filethread.cpp \
    fileutil.cpp \
    finddialog.cpp \
    searchthread.cpp \
    stringutil.cpp \
    taboutdialog.cpp \
    tfileinfo.cpp \
    tfilesystemmodel.cpp \
    thistory.cpp \
    dirform.cpp \
    main.cpp \
    mainwindow.cpp \
    tresultitemmodel.cpp \
    tsubwindow.cpp


HEADERS += \
    bookmarkMgr.h \
    copyoptions.h \
    copyprocessdialog.h \
    duplicateddialog.h \
    fileclipboard.h \
    filethread.h \
    fileutil.h \
    finddialog.h \
    searchthread.h \
    stringutil.h \
    taboutdialog.h \
    tfileinfo.h \
    tfilesystemmodel.h \
    thistory.h \
    dirform.h \
    mainwindow.h \
    tresultitemmodel.h \
    tsubwindow.h

FORMS += \
    copyprocessdialog.ui \
    dirform.ui \
    duplicateddialog.ui \
    finddialog.ui \
    mainwindow.ui \
    taboutdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
