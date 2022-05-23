cmake_minimum_required(VERSION 3.19)
project(server)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE STRING "LIBRARY OUTPUT DIRECTORY")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE STRING "RUNTIME OUTPUT DIRECTORY")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fcoroutines ")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)


add_subdirectory(shared)

include_directories(headers)

file(GLOB sources srcs/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

# find_package(fmt)
# target_link_libraries(${PROJECT_NAME} fmt::fmt)

target_link_libraries(${PROJECT_NAME} pthread uuid tcpsocket nlohmann DAL spdlog)