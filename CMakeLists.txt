cmake_minimum_required (VERSION 3.4)

project(tzlog_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#INCLUDE_DIRECTORIES(/root/Learning/c_c++/mimalloc/include/)
#FIND_LIBRARY(MIMALLOC_LIB mimalloc /root/Learning/c_c++/mimalloc_build/release)
#link_libraries(${MIMALLOC_LIB})
#link_libraries(/root/Learning/c_c++/mimalloc_build/release/libmimalloc.a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

add_executable(tzlog_test
    constexpr_fmt.cpp
    format_string.cpp
    runtime_logger.cpp
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # 新版本编译器要用 -pthread 而不是 -lpthread
    # 否则编译时会报错找不到pthread_atfork “undefined reference to pthread_atfork”
    target_link_libraries(tzlog_test -pthread )
    # The symbols are taken from the dynamic symbol table; 
    # you need the -rdynamic option to gcc, which makes it pass a flag to the 
    # linker which ensures that all symbols are placed in the table.
    target_link_libraries(tzlog_test -rdynamic )
    target_link_libraries(tzlog_test -lrt)
    #======================= 加入 tcmalloc 内存池支持(动态链接) ========================
    # target_link_libraries(test1 tcmalloc)
    # message(STATUS "tcmalloc(shared) support added")
    # ======================= 加入 jemalloc 内存池支持(动态链接) ========================
    #target_link_libraries(test1 jemalloc)
    #message(STATUS "jemalloc(shared) support added")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

