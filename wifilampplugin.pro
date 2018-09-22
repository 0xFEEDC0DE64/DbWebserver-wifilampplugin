QT += core network

DBLIBS += dbnetwork webserverlib

HEADERS += wifilampplugin.h \
    wifilampapplication.h \
    wifilampclient.h

SOURCES += wifilampplugin.cpp \
    wifilampapplication.cpp \
    wifilampclient.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS +=

OTHER_FILES += wifilampplugin.json

include(../plugin.pri)
