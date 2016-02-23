################################################################################
#Yume Engine MIT License (MIT)

# Copyright (c) 2015 arkenthera
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# File : CMakeLists.txt
# Date : 8.27.2015
# Comments :
################################################################################

cmake_minimum_required(VERSION 2.8.12.2)

set(YUME "YumeEngine")

PROJECT(${YUME})

set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMake"
  "${CMAKE_SOURCE_DIR}/CMake/Utils"
  "${CMAKE_SOURCE_DIR}/CMake/Packages"
)

#Include path variables
include(Paths)

#Use folders because they're cool looking!
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOSX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
endif()


#Determine architecture
if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()

  if(OS_MACOSX)
    # PROJECT_ARCH should be specified on Mac OS X.
    message(WARNING "No PROJECT_ARCH value specified, using ${PROJECT_ARCH}")
  endif()
endif()

if(OS_LINUX OR OS_MACOSX)
   set(YUME_CXX_COMPILER_FLAGS ${YUME_CXX_COMPILER_FLAGS} "-coverage")
endif()

if(OS_LINUX)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-msse YUME_GCC_HAS_SSE)
  if (YUME_GCC_HAS_SSE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
  endif ()

  check_cxx_compiler_flag(-msse YUME_GCC_HAS_SSE)
  if (YUME_GCC_HAS_SSE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
  endif ()

  set(YUME_WARNING_FLAGS "-Wno-deprecated -Wall -Wctor-dtor-privacy -Winit-self -Wno-overloaded-virtual -Wcast-qual -Wwrite-strings -Wextra -Wno-unused-parameter -pedantic -Wshadow -Wno-missing-field-initializers -Wno-long-long")
  # This is a set of sensible warnings that provide meaningful output

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${YUME_WARNING_FLAGS}")

  set(YUME_C_COMPILER_FLAGS        "-std=c99")
  set(YUME_CXX_COMPILER_FLAGS "-std=gnu++11")


  add_definitions(-fPIC)
endif ()

if(OS_MACOSX)
  set(YUME_COMPILER_FLAGS          "-fno-strict-aliasing -fstack-protector -funwind-tables -fvisibility=hidden")
  set(YUME_C_COMPILER_FLAGS        "-std=c99")
  set(YUME_CXX_COMPILER_FLAGS      "-fno-threadsafe-statics -fobjc-call-cxx-cdtors -fvisibility-inlines-hidden -std=c++11 -stdlib=libc++")
  set(YUME_COMPILER_FLAGS_DEBUG    "-O0 -g")
  set(YUME_COMPILER_FLAGS_RELEASE  "-O3")
  set(YUME_LINKER_FLAGS            "-Wl,-search_paths_first -Wl,-ObjC -Wl,-pie")
  set(YUME_LINKER_FLAGS_RELEASE    "-Wl,-dead_strip")

  # Standard libraries.
  set(YUME_STANDARD_LIBS "-lpthread")

  # Find the newest available base SDK.
  execute_process(COMMAND xcode-select --print-path OUTPUT_VARIABLE XCODE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
  foreach(OS_VERSION 10.10 10.9 10.8 10.7)
    set(SDK "${XCODE_PATH}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${OS_VERSION}.sdk")
    if(NOT "${CMAKE_OSX_SYSROOT}" AND EXISTS "${SDK}" AND IS_DIRECTORY "${SDK}")
      set(CMAKE_OSX_SYSROOT ${SDK})
    endif()
  endforeach()

  # Target SDK.
  set(YUME_TARGET_SDK               "10.8")
  set(YUME_COMPILER_FLAGS           "${CHI_COMPILER_FLAGS} -mmacosx-version-min=${YUME_TARGET_SDK}")
  set(CMAKE_OSX_DEPLOYMENT_TARGET  ${YUME_TARGET_SDK})

  # Target architecture.
  if(PROJECT_ARCH STREQUAL "x86_64")
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
  else()
    set(CMAKE_OSX_ARCHITECTURES "i386")
  endif()
endif()

set(LOG4CPLUS_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(LOG4CPLUS_BUILD_LOGGINGSERVER OFF CACHE BOOL "" FORCE)
set(LOG4CPLUS_ENABLE_DECORATED_LIBRARY_NAME OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(OS_WINDOWS)

  SET(YUME_COMPILER_FLAGS "/EHsc /Zi /GF /Gm /W4 /wd\"4100\" /wd\"4127\" /wd\"4244\" /wd\"4512\" /wd\"4701\" /wd\"4702\" /wd\"4996\" /wd\"4251\" /wd\"4275\" /wd\"4005\" /wd\"4267\"")
  SET(YUME_COMPILER_FLAGS_DEBUG "/MDd /RTC1 /Od")
  set(YUME_COMPILER_FLAGS_RELEASE  "/MD")
  set(YUME_LINKER_FLAGS_DEBUG      "/DEBUG")
  set(YUME_EXE_LINKER_FLAGS        "/MANIFEST:NO")
  add_definitions(-DWIN32 -D_WIN32 -D_WINDOWS -DUNICODE -D_UNICODE -DNOMINMAX -D_WIN32_WINNT=0x602)
endif()

include(Macros)
include(CMakeDependentOption)
find_package(OpenGL)
find_package(DirectX)

CMAKE_DEPENDENT_OPTION(YUME_BUILD_DIRECT3D11 "Build Direct3D 11" ON "WIN32;DirectX_D3D11_FOUND" OFF)
CMAKE_DEPENDENT_OPTION(YUME_BUILD_DIRECT3D12 "Build Direct3D 12" ON "WIN32;DirectX_D3D12_FOUND" OFF)
CMAKE_DEPENDENT_OPTION(YUME_BUILD_OPENGL "Build OpenGL" ON "OPENGL_FOUND" OFF)
option(YUME_USE_PCH "Use Precompiled header" OFF)

if(OS_WINDOWS)
  set(YUME_USE_PCH 1)
endif()

set(YUME_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(YUME_LIBRARIES ${YUME})



set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${YUME_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${YUME_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${YUME_BINARY_DIR}/bin")

INCLUDE(CMakeBuildSettings)

set(CMAKE_C_FLAGS                     "${YUME_COMPILER_FLAGS} ${YUME_C_COMPILER_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG               "${YUME_COMPILER_FLAGS_DEBUG} ${YUME_C_COMPILER_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE             "${YUME_COMPILER_FLAGS_RELEASE} ${YUME_C_COMPILER_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS                   "${YUME_COMPILER_FLAGS} ${YUME_CXX_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG             "${YUME_COMPILER_FLAGS_DEBUG} ${YUME_CXX_COMPILER_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE           "${YUME_COMPILER_FLAGS_RELEASE} ${YUME_CXX_COMPILER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS            "${YUME_LINKER_FLAGS} ${YUME_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG      "${YUME_LINKER_FLAGS_DEBUG} ${YUME_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${YUME_LINKER_FLAGS_RELEASE} ${YUME_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS         "${YUME_LINKER_FLAGS} ${YUME_SHARED_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "${YUME_LINKER_FLAGS_DEBUG} ${YUME_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${YUME_LINKER_FLAGS_RELEASE} ${YUME_SHARED_LINKER_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBOOST_ALL_NO_LIB=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_ALL_NO_LIB=1")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

ADD_SUBDIRECTORY(Engine)
ADD_SUBDIRECTORY(Samples)

message(STATUS "*** CONFIGURATION SETTINGS ***")
message(STATUS "Generator:                    ${CMAKE_GENERATOR}")
message(STATUS "Platform:                     ${CMAKE_SYSTEM_NAME}")
message(STATUS "Project architecture:         ${PROJECT_ARCH}")


if(${CMAKE_GENERATOR} STREQUAL "Ninja" OR ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  message(STATUS "Build type:                   ${CMAKE_BUILD_TYPE}")
endif()

message(STATUS "Standard libraries:           ${LIBRARIES}")

get_directory_property(DEFINITIONS COMPILE_DEFINITIONS)
message(STATUS "Compiler definitions:         ${DEFINITIONS}")

message(STATUS "C_FLAGS:                      ${CMAKE_C_FLAGS}")
message(STATUS "C_FLAGS_DEBUG:                ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C_FLAGS_RELEASE:              ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CXX_FLAGS:                    ${CMAKE_CXX_FLAGS}")
message(STATUS "CXX_FLAGS_DEBUG:              ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CXX_FLAGS_RELEASE:            ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "EXE_LINKER_FLAGS:             ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "EXE_LINKER_FLAGS_DEBUG:       ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
message(STATUS "EXE_LINKER_FLAGS_RELEASE:     ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message(STATUS "SHARED_LINKER_FLAGS:          ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "SHARED_LINKER_FLAGS_DEBUG:    ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
message(STATUS "SHARED_LINKER_FLAGS_RELEASE:  ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
