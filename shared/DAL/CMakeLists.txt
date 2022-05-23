include_directories(
    headers
)

file(GLOB sources srcs/*.cpp)
add_library(DAL ${sources})

target_link_libraries(DAL
    PRIVATE
        # fmt::fmt
        spdlog::spdlog
        event
		soci_core
		soci_postgresql
		pqxx
)


add_library(DAL::DAL ALIAS DAL)
target_include_directories(DAL
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
        $<INSTALL_INTERFACE:headers>)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#   add_subdirectory(test)
# endif()
