cmake_minimum_required(VERSION 3.15.3)
project(MidiUtils)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Utilities)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Dependencies and depenency configuration.

set(MIDIUTILS_DEPENDENCY_DIR "${CMAKE_CURRENT_LIST_DIR}/third_party" CACHE PATH "")

# Hedley - Cross platform macros
set(HEDLEY_SOURCE_DIR "${MIDIUTILS_DEPENDENCY_DIR}/hedley" CACHE PATH "")
using_library(hedley)

# Lua - Scrpting
set(LUA_LANGUAGE_CPP TRUE CACHE BOOL "")
set(LUA_SOURCE_DIR "${MIDIUTILS_DEPENDENCY_DIR}/lua" CACHE PATH "")
using_library(lua)

# LuaWrapper - Lua utilities for C++
set(LUAWRAPPER_SOURCE_DIR "${MIDIUTILS_DEPENDENCY_DIR}/luawrapper" CACHE PATH "")
using_library(luawrapper)

# rtmidi - Midi input and output
set(RTMIDI_BUILD_STATIC_LIBS ON CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "")
using_library(rtmidi)
set_target_properties(rtmidi PROPERTIES FOLDER rtmidi)
# This is a hack - I don't know why I need this.
include_directories("${MIDIUTILS_DEPENDENCY_DIR}/rtmidi")

# spdlog - Logging utiltites
using_library(spdlog)

################################################################################
# Game target and configuration.

# TODO: Have this automatically configured per-platform.
set(platform "windows") 

set(MIDIUTILS_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(MIDIUTILS_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

set(MIDIUTILS_SOURCES
    "${MIDIUTILS_SOURCE_DIR}/main/main.cpp"

    "${MIDIUTILS_SOURCE_DIR}/midiutils/midifileeventproducer.cpp"
    "${MIDIUTILS_SOURCE_DIR}/midiutils/midiread.cpp"
    "${MIDIUTILS_SOURCE_DIR}/midiutils/midiwrite.cpp"

    "${MIDIUTILS_SOURCE_DIR}/midiutils/${platform}/midistreamimpl.cpp" 

    "${MIDIUTILS_SOURCE_DIR}/luamidiutils/luaeventproducer.cpp"
    "${MIDIUTILS_SOURCE_DIR}/luamidiutils/luamidienums.cpp"
    "${MIDIUTILS_SOURCE_DIR}/luamidiutils/luamidiutils.cpp"

    "${MIDIUTILS_SOURCE_DIR}/midiutils/${platform}/midistreamimpl.cpp"

    "${MIDIUTILS_INCLUDE_DIR}/midiutils.hpp"
    "${MIDIUTILS_INCLUDE_DIR}/luamidiutils.hpp"
)

#source_group(TREE ${MIDIUTILS_SOURCE_DIR} FILES ${MIDIUTILS_SOURCES})
add_executable(MidiUtils ${MIDIUTILS_SOURCES})
target_include_directories(MidiUtils
    PRIVATE
        "${MIDIUTILS_SOURCE_DIR}"
        "${MIDIUTILS_INCLUDE_DIR}"
)

set(MIDIUTILS_ASSETS_DIR "${CMAKE_CURRENT_LIST_DIR}/data")

set_target_properties(MidiUtils
    PROPERTIES
        FOLDER MidiUtils
        CXX_STANDARD 20
        VS_DEBUGGER_WORKING_DIRECTORY "${MIDIUTILS_ASSETS_DIR}"
)

target_link_libraries(MidiUtils
    PRIVATE
        hedley
        lua
        luawrapper
        rtmidi
        spdlog
)
