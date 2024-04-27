cmake_minimum_required(VERSION 3.26)
set(PROG_NAME "zad4")
set(PROG_SRCS
    main.cpp
    )

project(zad3)

set(CMAKE_CXX_STANDARD 17)

add_executable(zad3 ${PROG_SRCS})

#target_link_libraries(zad3 ws2_32 Ws2_32)