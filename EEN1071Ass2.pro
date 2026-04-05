#-------------------------------------------------------------------------
# EEN1071 Assignment 2 - Project Configuration File
# This file defines the modules, source files, and libraries used by qmake.
#-------------------------------------------------------------------------

# Add essential Qt modules: core logic, GUI elements, and printing capabilities
QT       += core gui widgets printsupport

# Ensure compatibility with newer Qt versions by explicitly adding the widgets module
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Set the C++ standard to C++17 for modern features and syntax
CONFIG += c++17

# Uncommenting the line below helps maintain code quality by preventing
# the use of outdated (deprecated) Qt functions.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# List of all C++ source files to be compiled into the application
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

# List of header files containing class definitions and declarations
HEADERS += \
    mainwindow.h \
    qcustomplot.h

# The XML-based UI file designed in Qt Designer
FORMS += \
    mainwindow.ui

# External Library Linking: Links the Paho MQTT C client library
# (Required for IoT/Messaging functionality in this assignment)
LIBS  += -lpaho-mqtt3c

# Deployment rules: Defines where the binary is installed on different OS targets
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md
