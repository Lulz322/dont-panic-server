add_library(nlohmann INTERFACE)

add_library(nlohmann::nlohmann ALIAS nlohmann)
target_include_directories(nlohmann
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)
