set(HEADERS
    wifilampapplication.h
    wifilampclient.h
    wifilampplugin.h
)

set(SOURCES
    wifilampapplication.cpp
    wifilampclient.cpp
    wifilampplugin.cpp
)

add_library(wifilampplugin SHARED ${HEADERS} ${SOURCES})

target_link_libraries(wifilampplugin Qt5::Core Qt5::Network webserverlib)
