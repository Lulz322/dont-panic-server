include_directories(
    headers
)

file(GLOB sources srcs/*.cpp)
add_library(tcpsocket ${sources})

target_link_libraries(tcpsocket
    PRIVATE
        # fmt::fmt
        spdlog::spdlog
        event
)

add_library(tcpsocket::tcpsocket ALIAS tcpsocket)
target_include_directories(tcpsocket
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
        $<INSTALL_INTERFACE:headers>)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#   add_subdirectory(test)
# endif()
