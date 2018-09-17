QT += core network

DBLIBS += webserverlib

HEADERS += wifilampplugin.h \
    wifilampapplication.h

SOURCES += wifilampplugin.cpp \
    wifilampapplication.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS +=

OTHER_FILES += wifilampplugin.json

include(../plugin.pri)
