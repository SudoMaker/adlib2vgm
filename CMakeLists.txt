cmake_minimum_required(VERSION 3.14)
project(adlib2vgm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CPM_DOWNLOAD_VERSION 0.27.5)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
        NAME cxxopts
        GITHUB_REPOSITORY jarro2783/cxxopts
        VERSION 3.0.0
        GIT_TAG 2d8e17c4f88efce80e274cb03eeb902e055a91d3
)

set (CMAKE_CXX_FLAGS "-DDEBUG -O3 -W -Wall")
set (CMAKE_EXE_LINKER_FLAGS "-static -s")

include_directories(${cxxopts_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES_BINIO "binio/*.*")
file(GLOB_RECURSE SOURCES_FMTS "Formats/*.*")
file(GLOB_RECURSE SOURCES_FAKE_ADPLUG "FakeAdplug/*.*")

add_executable(adlib2vgm
        ${SOURCES_BINIO}
        ${SOURCES_FAKE_ADPLUG}
        ${SOURCES_FMTS}
        main.cpp CommonIncludes.hpp
        VgmOpl.cpp VgmOpl.hpp
        )

target_link_libraries(adlib2vgm stdc++fs)
