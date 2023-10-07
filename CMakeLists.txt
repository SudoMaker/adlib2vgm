cmake_minimum_required(VERSION 3.14)
project(adlib2vgm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "-DDEBUG -O3 -W -Wall")
set (CMAKE_EXE_LINKER_FLAGS "-static -s")

file(GLOB_RECURSE SOURCES_BINIO "binio/*.*")
file(GLOB_RECURSE SOURCES_FMTS "Formats/*.*")
file(GLOB_RECURSE SOURCES_FAKE_ADPLUG "FakeAdplug/*.*")

add_executable(adlib2vgm
        ${SOURCES_BINIO}
        ${SOURCES_FAKE_ADPLUG}
        ${SOURCES_FMTS}
        main.cpp
        VgmOpl.cpp
        )

target_link_libraries(adlib2vgm stdc++fs)
