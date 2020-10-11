cmake_minimum_required(VERSION 3.5)
project(Lua VERSION 5.4.0)

################################################################################
# Configuration settings.

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(LUA_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "The Lua library source directory.")
set(LUA_LANGUAGE_CPP FALSE CACHE BOOL "Compile the library in C++")
set(LUA_CONF_32BITS FALSE CACHE BOOL "Enables Lua with 32-bit integers and 32-bit floats.")
set(LUA_CONF_USE_C89 FALSE CACHE BOOL "Controls the use of non-ISO-C89 features. Define it if you want Lua to avoid the use of a few C99 features or Windows-specific features on Windows.")
set(LUA_CONF_COERSION_TO_STRING TRUE CACHE BOOL "Control how Lua performs some coercions. If ON, turns on automatic coercion from strings to numbers.")
set(LUA_CONF_COERSION_TO_NUMBER TRUE CACHE BOOL "Control how Lua performs some coercions. If ON, turns on automatic coercion from numbers to strings.")
set(LUA_CONF_USE_APICHECK FALSE CACHE BOOL "Turns on several consistency checks on the C API. Define it as a help when debugging C code.")

################################################################################
# Files

set(LUA_PUBLIC_HEADERS
    ${LUA_SOURCE_DIR}/lauxlib.h
    ${LUA_SOURCE_DIR}/lua.h
    ${LUA_SOURCE_DIR}/lualib.h
)

set(LUA_MODULE_SOURCES
    ${LUA_SOURCE_DIR}/lbaselib.c
    ${LUA_SOURCE_DIR}/lcorolib.c
    ${LUA_SOURCE_DIR}/ldblib.c
    ${LUA_SOURCE_DIR}/liolib.c
    ${LUA_SOURCE_DIR}/lmathlib.c
    ${LUA_SOURCE_DIR}/loadlib.c
    ${LUA_SOURCE_DIR}/loslib.c
    ${LUA_SOURCE_DIR}/lstrlib.c
    ${LUA_SOURCE_DIR}/ltablib.c
    ${LUA_SOURCE_DIR}/lutf8lib.c
)

set(LUA_LIBRARY_SOURCES
    ${LUA_SOURCE_DIR}/lapi.c
    ${LUA_SOURCE_DIR}/lauxlib.c
    ${LUA_SOURCE_DIR}/lcode.c
    ${LUA_SOURCE_DIR}/lctype.c
    ${LUA_SOURCE_DIR}/ldebug.c
    ${LUA_SOURCE_DIR}/ldo.c
    ${LUA_SOURCE_DIR}/ldump.c
    ${LUA_SOURCE_DIR}/lfunc.c
    ${LUA_SOURCE_DIR}/lgc.c
    ${LUA_SOURCE_DIR}/linit.c
    ${LUA_SOURCE_DIR}/llex.c
    ${LUA_SOURCE_DIR}/lmem.c
    ${LUA_SOURCE_DIR}/lobject.c
    ${LUA_SOURCE_DIR}/lopcodes.c
    ${LUA_SOURCE_DIR}/lparser.c
    ${LUA_SOURCE_DIR}/lstate.c
    ${LUA_SOURCE_DIR}/lstring.c
    ${LUA_SOURCE_DIR}/ltable.c
    ${LUA_SOURCE_DIR}/ltests.c
    ${LUA_SOURCE_DIR}/ltm.c
    ${LUA_SOURCE_DIR}/lundump.c
    ${LUA_SOURCE_DIR}/lvm.c
    ${LUA_SOURCE_DIR}/lzio.c
)

set(LUA_LIBRARY_HEADERS
    ${LUA_SOURCE_DIR}/lapi.h
    ${LUA_SOURCE_DIR}/lcode.h
    ${LUA_SOURCE_DIR}/lctype.h
    ${LUA_SOURCE_DIR}/ldebug.h
    ${LUA_SOURCE_DIR}/ldo.h
    ${LUA_SOURCE_DIR}/lfunc.h
    ${LUA_SOURCE_DIR}/lgc.h
    ${LUA_SOURCE_DIR}/ljumptab.h
    ${LUA_SOURCE_DIR}/llex.h
    ${LUA_SOURCE_DIR}/llimits.h
    ${LUA_SOURCE_DIR}/lmem.h
    ${LUA_SOURCE_DIR}/lobject.h
    ${LUA_SOURCE_DIR}/lopcodes.h
    ${LUA_SOURCE_DIR}/lparser.h
    ${LUA_SOURCE_DIR}/lprefix.h
    ${LUA_SOURCE_DIR}/lstate.h
    ${LUA_SOURCE_DIR}/lstring.h
    ${LUA_SOURCE_DIR}/ltable.h
    ${LUA_SOURCE_DIR}/ltests.h
    ${LUA_SOURCE_DIR}/ltm.h
    ${LUA_SOURCE_DIR}/luaconf.h
    ${LUA_SOURCE_DIR}/lundump.h
    ${LUA_SOURCE_DIR}/lvm.h
    ${LUA_SOURCE_DIR}/lzio.h
)

set(LUA_INTERPRETER_SOURCES
    ${LUA_SOURCE_DIR}/lua.c
)

source_group("Public Headers" FILES ${LUA_PUBLIC_HEADERS})
source_group("Modules" FILES ${LUA_MODULE_SOURCES})
source_group("Library/Source" FILES ${LUA_LIBRARY_SOURCES})
source_group("Library/Header" FILES ${LUA_LIBRARY_HEADERS})
source_group("Interpreter" FILES ${LUA_INTERPRETER_SOURCES})

################################################################################
# Apply luaconf settings

if(${LUA_CONF_32BITS})
    set(LUA_CONF_DEFINES -DLUA_32BITS)
endif()

if(${LUA_CONF_USE_C89})
    set(LUA_CONF_DEFINES -DLUA_USE_C89 ${LUA_CONF_DEFINES})
endif()

if(NOT ${LUA_CONF_COERSION_TO_STRING})
    set(LUA_CONF_DEFINES -DLUA_NOCVTN2S ${LUA_CONF_DEFINES})
endif()

if(NOT ${LUA_CONF_COERSION_TO_NUMBER})
    set(LUA_CONF_DEFINES -DLUA_NOCVTS2N ${LUA_CONF_DEFINES})
endif()

if(${LUA_CONF_USE_APICHECK})
    set(LUA_CONF_DEFINES -DLUA_USE_APICHECK ${LUA_CONF_DEFINES})
endif()

################################################################################
# Set warnings.

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" 
    OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Warnings valid for both C and C++
    set(LUA_CWARNSCPP
        -Wextra
        -Wshadow
        -Wsign-compare
        -Wundef
        -Wwrite-strings
        -Wredundant-decls
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wstrict-aliasing=3
        -Wno-aggressive-loop-optimizations
        -Wlogical-op
        -Werror
        # -pedantic   # warns if we use jump tables
        # the next warnings generate too much noise, so they are disabled
        # -Wconversion  -Wno-sign-conversion
        # -Wsign-conversion
        # -Wstrict-overflow=2
        # -Wformat=2
        # -Wcast-qual
    )

    # The next warnings are neither valid nor needed for C++
    set(LUA_CWARNSC
        -Wdeclaration-after-statement
        -Wmissing-prototypes
        -Wnested-externs
        -Wstrict-prototypes
        -Wc++-compat
        -Wold-style-definition
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # TODO: Add MSVC warnings
endif()

if (${LUA_LANGUAGE_CPP})
    set(LUA_CWARNS ${LUA_CWARNSC} ${LUA_CWARNSCPP})
    set(LUA_LINKER_LANGUAGE CXX)
else()
    set(LUA_CWARNS ${LUA_CWARNSC})
    set(LUA_LINKER_LANGUAGE C)
endif()

################################################################################
# Set up Lua targets.

# Library.
add_library(lua STATIC
    ${LUA_PUBLIC_HEADERS}
    ${LUA_MODULE_SOURCES}
    ${LUA_LIBRARY_SOURCES}
    ${LUA_LIBRARY_HEADERS}
)
target_include_directories(lua
    PUBLIC
        $<BUILD_INTERFACE:${LUA_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(lua 
    PUBLIC 
        ${LUA_CONF_DEFINES}
    PRIVATE
        ${LUA_CWARNS}
)
set_target_properties(lua 
    PROPERTIES
        LINKER_LANGUAGE ${LUA_LINKER_LANGUAGE}
        FOLDER Lua
)

# Interpreter.
add_executable(luai ${LUA_INTERPRETER_SOURCES})
set_target_properties(luai
    PROPERTIES
        LINKER_LANGUAGE ${LUA_LINKER_LANGUAGE}
        FOLDER Lua
)
target_link_libraries(luai PRIVATE lua)

################################################################################
# Installation instructions.

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/lua/BANANAN/)

install(TARGETS lua
    EXPORT lua-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    ${LUA_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export the targets to a script.
install(EXPORT lua-targets
  FILE
    LuaTargets.cmake
  NAMESPACE
    lua::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LuaConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/LuaConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LuaConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config, configversion and custom find modules.
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LuaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LuaConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT lua-targets 
    FILE ${CMAKE_CURRENT_BINARY_DIR}/LuaTargets.cmake
    NAMESPACE lua::
)
export(PACKAGE lua)
