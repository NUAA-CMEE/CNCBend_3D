#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T09:37:18
#
#-------------------------------------------------

QT       += core gui network
QT       += opengl printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CNCBend_3D
TEMPLATE += app


DEPENDPATH += libpng
INCLUDEPATH += . libpng


SOURCES += main.cpp\
        mainwindow.cpp \
    bendglwidget.cpp \
    sysglobalvars.cpp \
    drawtools.cpp \
    print3d.cpp \
    network.cpp \
    recordClient.cpp \
    uirealtimepage.cpp \
    uiselectpage.cpp \
    progressdlg.cpp \
    toolconfigdialog.cpp \
    toolinfoinput.cpp \
    uideploypage.cpp \
    uimachineconstant.cpp \
    CNCGlobalFuncs.cpp \
    uiaxisconfigdialog.cpp \
    uibendorderpage.cpp \
    filereader.cpp \
    fileoperate.cpp

HEADERS  += mainwindow.h \
    bendglwidget.h \
    SysGLData.h \
    sysglobalvars.h \
    SysKeyboard.h \
    drawtools.h \
    print3d.h \
    network.h \
    recordClient.h \
    recordCmd.h \
    uirealtimepage.h \
    uiselectpage.h \
    progressdlg.h \
    toolconfigdialog.h \
    toolinfoinput.h \
    uideploypage.h \
    uimachineconstant.h \
    CNCGlobalFuncs.h \
    sqlite3.h \
    uiaxisconfigdialog.h \
    uibendorderpage.h \
    filereader.h \
    fileoperate.h

FORMS    += mainwindow.ui \
    uirealtimepage.ui \
    uiselectpage.ui \
    progressdlg.ui \
    toolconfigdialog.ui \
    toolinfoinput.ui \
    uimachineconstant.ui \
    uiaxisconfigdialog.ui \
    uibendorderpage.ui

TARGET = ui
MOC_DIR = moc
OBJECTS_DIR = obj
LIBS += -lsqlite3
#LIBS += -LD:/QT-WIN-PROJECT/CNCBend_3D/libpng/libpng16d.lib
#LIBS += -lglut32
#LIBS += -LC:\glut

RESOURCES += \
    resource.qrc

OTHER_FILES +=
