cmake_minimum_required(VERSION 3.30)
project(nh35a VERSION 0.1 LANGUAGES C)

if(DEFINED NH_DEBUG_MEM AND NH_DEBUG_MEM STREQUAL "ON")
  add_definitions(-DNH_DEBUG_MEM)
endif()

add_library(nh35a
  btree.c
  heapq.c
  mem.c
  messages.c
  queue.c
  slist.c
  stack.c
  alg.c
  timer.c
)

file(GLOB NH_FILES "${CMAKE_SOURCE_DIR}/*.h")
install(FILES ${NH_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nh35a)
install(TARGETS nh35a DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

function(add_executable_tests)
  foreach(name IN LISTS ARGV)
    set(name ${name}-test)
    add_executable(${name} ${name}.c nh35a_test.c)
    target_link_libraries(${name} nh35a)
    add_test(NAME ${name} COMMAND ${name})
  endforeach()
endfunction()

enable_testing()

add_executable_tests(
  btree
  heapq
  queue
  slist
  alg
  stack
)

if(DEFINED NH_DEBUG_MEM AND NH_DEBUG_MEM STREQUAL "ON")
  add_executable_tests(mem)
endif()
