cmake_minimum_required(VERSION 3.20)

project(NiflyDLL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(NIFLY_TARGET "nifly" CACHE STRING "Name of the CMake target that provides the nifly dependency")
set(_nifly_default_root "${CMAKE_CURRENT_SOURCE_DIR}/external/nifly")
set(NIFLY_ROOT "${_nifly_default_root}" CACHE PATH "Path to a nifly source tree")
set(NIFLY_INCLUDE_DIR "" CACHE PATH "Directory that contains nifly headers (e.g. niffile.hpp)")
set(NIFLY_LIBRARY "" CACHE FILEPATH "Prebuilt nifly library to link against")

if(NOT TARGET ${NIFLY_TARGET})
    if(EXISTS "${NIFLY_ROOT}/CMakeLists.txt")
        message(STATUS "Using nifly sources from ${NIFLY_ROOT}")
        add_subdirectory(${NIFLY_ROOT} ${CMAKE_BINARY_DIR}/nifly EXCLUDE_FROM_ALL)
    endif()
endif()

if(NOT TARGET ${NIFLY_TARGET})
    if(NOT NIFLY_INCLUDE_DIR)
        find_path(NIFLY_INCLUDE_DIR
            NAMES niffile.hpp
            HINTS
                "${NIFLY_ROOT}/include"
                "${NIFLY_ROOT}"
            PATH_SUFFIXES include
        )
    endif()

    if(NOT NIFLY_LIBRARY)
        find_library(NIFLY_LIBRARY
            NAMES nifly
            HINTS
                "${NIFLY_ROOT}/lib"
                "${NIFLY_ROOT}"
            PATH_SUFFIXES lib lib64
        )
    endif()

    if(NIFLY_INCLUDE_DIR AND NIFLY_LIBRARY)
        add_library(${NIFLY_TARGET} UNKNOWN IMPORTED)
        set_target_properties(${NIFLY_TARGET} PROPERTIES
            IMPORTED_LOCATION "${NIFLY_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${NIFLY_INCLUDE_DIR}"
        )
    else()
        if(NOT EXISTS "${_nifly_default_root}/CMakeLists.txt")
            message(FATAL_ERROR
                "Unable to locate the nifly dependency. Initialize the bundled submodule with "
                "'git submodule update --init --recursive', provide a target named ${NIFLY_TARGET}, "
                "set NIFLY_ROOT to a nifly checkout, or specify NIFLY_INCLUDE_DIR and NIFLY_LIBRARY.")
        else()
            message(FATAL_ERROR
                "Unable to locate the nifly dependency. Provide a target named ${NIFLY_TARGET}, "
                "set NIFLY_ROOT to a nifly checkout, or specify NIFLY_INCLUDE_DIR and NIFLY_LIBRARY.")
        endif()
    endif()
endif()

set(nifly_dll_sources
    Anim.cpp
    Logger.cpp
    NiflyFunctions.cpp
    NiflyWrapper.cpp
)

set(nifly_dll_headers
    Anim.h
    Logger.hpp
    NiflyDefs.hpp
    NiflyFunctions.hpp
    NiflyWrapper.hpp
    pch.h
    framework.h
)

if(WIN32)
    list(APPEND nifly_dll_sources dllmain.cpp pch.cpp)
    list(APPEND nifly_dll_headers resource.h resource1.h NiflyDLL.rc)
endif()

add_library(NiflyDLL SHARED ${nifly_dll_sources} ${nifly_dll_headers})
set_target_properties(NiflyDLL PROPERTIES OUTPUT_NAME NiflyDLL)

if(APPLE)
    set_target_properties(NiflyDLL PROPERTIES PREFIX "")
endif()

if(NOT WIN32)
    target_compile_definitions(NiflyDLL PRIVATE NIFLYDLL_EXPORTS)
else()
    target_compile_definitions(NiflyDLL PRIVATE NIFLYDLL_EXPORTS WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

target_include_directories(NiflyDLL
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(NiflyDLL PRIVATE ${NIFLY_TARGET})

if(MSVC)
    target_compile_options(NiflyDLL PRIVATE /W4 /permissive-)
    target_precompile_headers(NiflyDLL PRIVATE pch.h)
else()
    target_compile_options(NiflyDLL PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${nifly_dll_sources} ${nifly_dll_headers})

install(TARGETS NiflyDLL
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../PyNifly
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../PyNifly
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../PyNifly)
