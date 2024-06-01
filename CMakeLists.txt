cmake_minimum_required(VERSION 3.14)
project(adlib2vgm LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "-DDEBUG -O3 -W -Wall -Wno-unused-parameter -Wno-implicit-fallthrough")
set (CMAKE_EXE_LINKER_FLAGS "-static -s")

include_directories("binio" "utfcpp" "zstr")

file(GLOB_RECURSE SOURCES_BINIO "binio/*.cpp")
file(GLOB_RECURSE SOURCES_FMTS "Formats/*.cpp")
file(GLOB_RECURSE SOURCES_FMTS_DEPS "deps/*.c")
file(GLOB_RECURSE SOURCES_FAKE_ADPLUG "FakeAdplug/*.cpp")

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
find_library(ZLIB "z")
if(NOT ZLIB)
	message(FATAL_ERROR "static zlib library not found")
endif()

add_executable(adlib2vgm
	${SOURCES_BINIO}
	${SOURCES_GZSTREAM}
	${SOURCES_FAKE_ADPLUG}
	${SOURCES_FMTS_DEPS}
	${SOURCES_FMTS}
	main.cpp
	VgmOpl.cpp
)

target_include_directories(adlib2vgm PRIVATE "deps")

target_link_libraries(adlib2vgm ${ZLIB})
