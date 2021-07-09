cmake_minimum_required(VERSION 3.13)
project(mymuduo C)

set(CMAKE_CXX_STANDARD 14)


add_library(Chapter02src
            Mutex.h
        Condition.h
        CountDownLatch.h
        Singleton.h)

set_target_properties(Chapter02src PROPERTIES LINKER_LANGUAGE CXX)

set(Chapter02 ${Chapter02src})


add_executable(test02 test.cpp)

target_link_libraries(test02 Chapter02)

