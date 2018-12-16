find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)

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

set(OTHER_FILES
    wifilampplugin.json
)

add_library(webserver-plugins-wifilampplugin SHARED ${HEADERS} ${SOURCES} ${OTHER_FILES})

set_target_properties(webserver-plugins-wifilampplugin PROPERTIES OUTPUT_NAME wifilampplugin PREFIX "")

target_link_libraries(webserver-plugins-wifilampplugin Qt5::Core Qt5::Network webserverlib)

add_dependencies(webserver-plugins webserver-plugins-wifilampplugin)
