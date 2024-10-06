cmake_minimum_required(VERSION 3.30)
project(nh35a VERSION 0.1 LANGUAGES C)

if(DEFINED NH_DEBUG_MEM AND NH_DEBUG_MEM STREQUAL "ON")
  add_definitions(-DNH_DEBUG_MEM)
endif()

add_library(nh35a
  heapq.c
  mem.c
  slist.c
)

function(add_test_executable name)
  add_executable(${name} ${name}.c)
  target_link_libraries(${name} nh35a)
endfunction()

add_test_executable(heapq-test)
add_test_executable(slist-test)
