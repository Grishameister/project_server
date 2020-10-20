set(test_name project_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${includes})

file(GLOB tests "${PROJECT_SOURCE_DIR}/test/*.cpp")

add_executable(${test_name} ${tests} tests.cpp main.cpp)

target_link_libraries(${test_name} ${library_name} DBAccess_lib gmock_main gmock -pthread)

add_test(NAME ${test_name} COMMAND ${test_name})