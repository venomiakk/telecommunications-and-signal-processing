cmake_minimum_required(VERSION 3.26)
set(PROG_NAME "zad3")
set(PROG_SRCS
    main.cpp
    fileIO.cpp
    fileIO.h
    server.cpp
    server.h
    client.cpp
    client.h
    huffman.cpp
    huffman.h
    )

project(zad3)

set(CMAKE_CXX_STANDARD 17)

add_executable(zad3 ${PROG_SRCS})

#if(MINGW)
#    target_link_libraries(YourTargetName ws2_32)
#endif()

target_link_libraries(zad3 ws2_32 Ws2_32)