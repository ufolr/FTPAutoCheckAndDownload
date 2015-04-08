#-------------------------------------------------
#
# Project created by QtCreator 2015-03-15T13:11:25
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoFTPDownLoader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ftpclient.cpp \
    ftpdirdownloader.cpp \
    sqllteinfo.cpp \
    setting.cpp \
    taskmanager.cpp \
    downloadtaskinfo.cpp \
    tasksetdialog.cpp \
    qtsingleapplication/src/qtlocalpeer.cpp \
    qtsingleapplication/src/qtlockedfile.cpp \
    qtsingleapplication/src/qtlockedfile_unix.cpp \
    qtsingleapplication/src/qtlockedfile_win.cpp \
    qtsingleapplication/src/qtsingleapplication.cpp \
    tray.cpp

HEADERS  += mainwindow.h \
    ftpclient.h \
    ftpdirdownloader.h \
    sqllteinfo.h \
    setting.h \
    taskmanager.h \
    downloadtaskinfo.h \
    tasksetdialog.h \
    qtsingleapplication/src/qtlocalpeer.h \
    qtsingleapplication/src/QtLockedFile \
    qtsingleapplication/src/qtlockedfile.h \
    qtsingleapplication/src/QtSingleApplication \
    qtsingleapplication/src/qtsingleapplication.h \
    tray.h

FORMS    += mainwindow.ui \
    downloadtaskinfo.ui \
    tasksetdialog.ui

DISTFILES += \
    qtsingleapplication/src/qtsingleapplication.pri \
    qtsingleapplication/src/qtsinglecoreapplication.pri

RC_FILE = downloader.rc
