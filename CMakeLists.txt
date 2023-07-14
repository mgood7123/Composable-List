cmake_minimum_required(VERSION 3.4.3)
project(Composable_List)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT COMMAND testBuilder_build)
    add_subdirectory(testBuilder)
    testBuilder_set_current_working_directory_to_default_binary_directory()
endif()

testBuilder_add_source(Composable_List src/empty.cpp)
testBuilder_add_include(Composable_List include)
testBuilder_build_shared_library(Composable_List)

