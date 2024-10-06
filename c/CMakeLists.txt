cmake_minimum_required(VERSION 3.30)
project(nh35a VERSION 0.1 LANGUAGES C)

if(DEFINED NH_DEBUG_MEM AND NH_DEBUG_MEM STREQUAL "ON")
  add_definitions(-DNH_DEBUG_MEM)
endif()

add_library(nh35a
  btree.c
  heapq.c
  mem.c
  slist.c
)

function(add_executable_tests)
  foreach(name IN LISTS ARGV)
    set(name ${name}-test)
    add_executable(${name} ${name}.c nh35a_test.c)
    target_link_libraries(${name} nh35a)
  endforeach()
endfunction()

add_executable_tests(
  btree
  slist
)
