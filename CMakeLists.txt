CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(YumeEngine)

set(CMAKE_MODULE_PATH 
  "${CMAKE_SOURCE_DIR}/CMake"
  "${CMAKE_SOURCE_DIR}/CMake/Utils" 
  "${CMAKE_SOURCE_DIR}/CMake/Packages"
)


SET(YUME_TEMPLATES_DIR "${CMAKE_SOURCE_DIR}/CMake/Templates")

include(CMakeDependentOption)

OPTION(YUME_STATIC "Static Library" ON)

include_directories("${CMAKE_SOURCE_DIR}/Yume/include")
include_directories("${CMAKE_BINARY_DIR}/include")

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

