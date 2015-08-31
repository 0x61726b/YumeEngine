CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.4)
  cmake_policy(SET CMP0003 NEW)
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.4)

PROJECT(YumeEngine)

set(CMAKE_MODULE_PATH 
  "${CMAKE_SOURCE_DIR}/CMake"
  "${CMAKE_SOURCE_DIR}/CMake/Utils" 
  "${CMAKE_SOURCE_DIR}/CMake/Packages"
)


SET(YUME_TEMPLATES_DIR "${CMAKE_SOURCE_DIR}/CMake/Templates")

include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

OPTION(YUME_STATIC "Static Library" ON)

include_directories("${CMAKE_SOURCE_DIR}/Yume/include")
include_directories("${CMAKE_BINARY_DIR}/include")

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

include(CheckTypeSize)
CHECK_TYPE_SIZE("void*" YUME_PTR_SIZE BUILTIN_TYPES_ONLY)
if (YUME_PTR_SIZE EQUAL 8)
  set(YUME_PLATFORM_X64 TRUE)
else ()
  set(YUME_PLATFORM_X64 FALSE)
endif ()


if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  check_cxx_compiler_flag(-msse YUME_GCC_HAS_SSE)
  if (YUME_GCC_HAS_SSE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
  endif ()
  # This is a set of sensible warnings that provide meaningful output
  set(YUME_WARNING_FLAGS "-Wno-deprecated -Wall -Wctor-dtor-privacy -Winit-self -Wno-overloaded-virtual -Wcast-qual -Wwrite-strings -Wextra -Wno-unused-parameter -pedantic -Wshadow -Wno-missing-field-initializers -Wno-long-long")
  if (NOT APPLE)
      set(YUME_WARNING_FLAGS "${OGRE_WARNING_FLAGS} -Wno-unused-but-set-parameter")
  endif ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${YUME_WARNING_FLAGS}")
endif ()
if (MSVC)
  if (CMAKE_BUILD_TOOL STREQUAL "nmake")
    # set variable to state that we are using nmake makefiles
	set(NMAKE TRUE)
  endif ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
  # Enable intrinsics on MSVC in debug mode
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi")
  if (CMAKE_CL_64)
    # Visual Studio bails out on debug builds in 64bit mode unless
	# this flag is set...
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
  endif ()
endif()

if ((CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX) AND NOT MINGW)
  # Test for GCC visibility
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-fvisibility=hidden YUME_GCC_VISIBILITY)
  if (YUME_GCC_VISIBILITY)
    # determine gcc version
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion 
      OUTPUT_VARIABLE YUME_GCC_VERSION)
    message(STATUS "Detected g++ ${OGRE_GCC_VERSION}")
    message(STATUS "Enabling GCC visibility flags")
    set(YUME_GCC_VISIBILITY_FLAGS "-DYUME_GCC_VISIBILITY -fvisibility=hidden")
    set(XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
  
    # check if we can safely add -fvisibility-inlines-hidden
    string(TOLOWER "${CMAKE_BUILD_TYPE}" YUME_BUILD_TYPE)
    if (YUME_BUILD_TYPE STREQUAL "debug" AND YUME_GCC_VERSION VERSION_LESS "4.2")
      message(STATUS "Skipping -fvisibility-inlines-hidden due to possible bug in g++ < 4.2")
    else ()
        set(YUME_GCC_VISIBILITY_FLAGS "${YUME_GCC_VISIBILITY_FLAGS} -fvisibility-inlines-hidden")
        set(XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES")
    endif ()
  endif (YUME_GCC_VISIBILITY)
  
  # Fix x64 issues on Linux
  if(YUME_PLATFORM_X64 AND NOT APPLE)
    add_definitions(-fPIC)
  endif()  
endif ((CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX) AND NOT MINGW)


INCLUDE(FindLibraries)

CMAKE_DEPENDENT_OPTION(YUME_BUILD_DIRECT3D11 "Build Direct3D 11" ON "WIN32;DirectX_D3D11_FOUND" OFF)
CMAKE_DEPENDENT_OPTION(YUME_BUILD_DIRECT3D12 "Build Direct3D 12" ON "WIN32;DirectX_D3D12_FOUND" OFF)
CMAKE_DEPENDENT_OPTION(YUME_BUILD_OPENGL "Build OpenGL" ON "OPENGL_FOUND" OFF)

set(YUME_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(YUME_LIBRARIES YUME)




set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${YUME_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${YUME_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${YUME_BINARY_DIR}/bin")

INCLUDE(CMakeBuildSettings)

ADD_SUBDIRECTORY(Samples)

ADD_SUBDIRECTORY(Yume)

ADD_SUBDIRECTORY(Renderers)

ADD_SUBDIRECTORY(CMake)

