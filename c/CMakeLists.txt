cmake_minimum_required(VERSION 3.30)
project(nh35a VERSION 0.1 LANGUAGES C)

add_library(nh35a OBJECT nh35a.c)

add_executable(nh35a-test nh35a-test.c)
target_link_libraries(nh35a-test nh35a)
