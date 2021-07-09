cmake_minimum_required(VERSION 3.13)
project(mymuduo)

set(CMAKE_CXX_STANDARD 14)

#set(QueSrc
  #      LockFreeList.h)

#add_library(src ${QueSrc})

add_executable(test test.cpp)

#target_link_libraries(test src)