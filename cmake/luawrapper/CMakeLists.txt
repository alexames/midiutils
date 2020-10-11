cmake_minimum_required(VERSION 3.5)
project(LuaWrapper)

set(LUAWRAPPER_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "The LuaWrapper library source directory.")

add_library(luawrapper INTERFACE)
target_include_directories(luawrapper
    INTERFACE
        $<BUILD_INTERFACE:${LUAWRAPPER_SOURCE_DIR}>
)