cmake_minimum_required(VERSION 3.15)
project(project_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Werror -Wpedantic -std=c++1z -g -I msgpack-c/include -pthread -fPIC")

find_package(Threads)

set(project_name project_server)
set(library_name server_lib)

configure_file(CMakeLists.txt.in
        googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build)

set(includes ${PROJECT_SOURCE_DIR}/include)
include_directories(${includes})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${library_name} SHARED ${sources})

add_subdirectory(DBAccess)

add_executable(project_server main.cpp)
target_link_libraries(${project_name} ${library_name} DBAccess_lib)

enable_testing()
add_subdirectory(test)
