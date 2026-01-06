QT       += core gui widgets sql printsupport charts
CONFIG   += c++17
VERSION  = 1.0.0

TARGET = EmployeeManagementSystemQt
TEMPLATE = app

SOURCES += main_qt.cpp \
           MainWindow.cpp \
           Employee.cpp \
           Time.cpp \
           SettingsDialog.cpp \
           LoginDialog.cpp \
           RegisterDialog.cpp \
           ResetPasswordDialog.cpp \
           ProfileDialog.cpp \
           AdminDialog.cpp \
           MyReportsDialog.cpp \
           AdminReportsDialog.cpp

HEADERS += MainWindow.h \
           Employee.h \
           Time.h \
           SettingsDialog.h \
           LoginDialog.h \
           RegisterDialog.h \
           ResetPasswordDialog.h \
           ProfileDialog.h \
           AdminDialog.h \
           Utils.h \
           MyReportsDialog.h \
           AdminReportsDialog.h

RESOURCES += resources.qrc

win32 {
    RC_ICONS = icon.ico
}