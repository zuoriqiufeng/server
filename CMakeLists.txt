cmake_minimum_required(VERSION 3.9)
project(server)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -pthread")

#find_package(yaml-cpp CONFIG REQUIRED)
message(DEBUG  "${YAML_CPP_INCLUDE_DIR}")

include_directories(${PROJECT_SOURCE_DIR})

find_package (PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

include_directories(${YAML_CPP_INCLUDE_DIR})
link_libraries(${YAML_CPP_LIBRARIES})

# link_libraries(/usr/local/lib/libyaml-cpp.so)
file(GLOB LIB_SRC "src/*.cpp")
# 
add_library(server SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(server)


link_libraries(server)
# add_executable(test1 tests/test.cpp)
# add_dependencies(test1 server)
# target_link_libraries(test1 server)

add_executable(test_kama tests/test_kama.cpp)
add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config server)
force_redefine_file_macro_for_sources(test_config)
# target_link_libraries(server ${YAML_CPP_LIBRARIES})
#target_link_libraries(test_config server)


add_executable(test_thread tests/test_thread.cpp)
add_executable(test_util tests/test_util.cpp)
#add_dependencies(test_thread server)
force_redefine_file_macro_for_sources(test_thread)
# target_link_libraries(test_thread server)

SET(CMAKE_CACHEFILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
SET(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


