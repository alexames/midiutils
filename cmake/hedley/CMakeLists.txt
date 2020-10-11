cmake_minimum_required(VERSION 3.5)
project(Hedley)

set(HEDLEY_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "The Hedley library source directory.")

add_library(hedley INTERFACE)
target_include_directories(hedley
    INTERFACE
        $<BUILD_INTERFACE:${HEDLEY_SOURCE_DIR}>
)