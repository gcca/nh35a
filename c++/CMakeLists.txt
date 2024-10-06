cmake_minimum_required(VERSION 3.25)
project(nh35a VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(GTest REQUIRED)
enable_testing()
include(GoogleTest)

add_executable(nh35a-test nh35a-test.cc)
target_link_libraries(nh35a-test GTest::gtest_main)
gtest_discover_tests(nh35a-test)
