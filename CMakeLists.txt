project(YumeEngine)

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/CMake
    ${CMAKE_MODULE_PATH})


include(Common)
include(Paths)

find_package(D3D REQUIRED)
find_package(Boost REQUIRED)
find_package(DXUT REQUIRED)

# configure
set(D3D_LIBS
    ${D3D_D3D11_LIBRARY}
    ${D3D_D3DCompiler_LIBRARY}
    ${WINDOWS_SDK_USP10_LIBRARY}
    ${DXUT_LIBRARIES})

add_definitions(-D UNICODE)
add_definitions(-D NOMINMAX)
add_definitions(-D _WIN32_WINNT=0x0601)

add_subdirectory(Engine)
add_subdirectory(Samples)
# dune
# file(GLOB_RECURSE ext_src src/ext/*)
# source_group("Ext" FILES ${ext_src})
# file(GLOB dune_src Engine/Source/Runtime/Renderer/* ${ext_src})
#
# set(dune_dir ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Runtime/Renderer)
#
# list(REMOVE_ITEM dune_src
#     ${dune_dir}/sdk_mesh.cpp
#     ${dune_dir}/sdk_mesh.h)
#
# add_library(dune ${dune_src})
#
# shaders
file(GLOB hlsl_src Engine/Assets/Shaders/*.hlsl Engine/Assets/Shaders/common.h)
source_group("Shader Files" FILES ${hlsl_src})
set_source_files_properties(${hlsl_src} HEADER_FILE_ONLY TRUE EXTERNAL_OBJECT TRUE)

set(cso_dir ${CMAKE_BINARY_DIR}/cso)
file(MAKE_DIRECTORY ${cso_dir})

add_custom_target(cso
    ALL
    COMMAND ${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders/compile.bat ${cso_dir}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders
    DEPENDS ${cso_dir})

add_custom_target(shaders
    COMMAND ${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders/compile.bat ${cso_dir}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Engine/Assets/Shaders
    SOURCES ${hlsl_src})

# # common files
# set(common_src src/pppipe.cpp src/pppipe.h src/common_gui.cpp src/common_gui.h src/common_renderer.cpp src/common_renderer.h src/common_dxut.cpp src/common_dxut.h)
#
# # lpv
# add_executable(lpv WIN32 src/main_gi.cpp src/skydome.h ${common_src})
# set_target_properties (lpv PROPERTIES COMPILE_DEFINITIONS "LPV")
# target_link_libraries(lpv
#     dune
#     ${D3D_LIBS}
#     ${Assimp_LIBRARY}
#     comctl32.lib)
#
# # vct
# add_executable(vct WIN32 src/main_gi.cpp src/skydome.h ${common_src})
# target_link_libraries(vct
#     dune
#     ${D3D_LIBS}
#     ${Assimp_LIBRARY}
#     comctl32.lib)


# install
# install(TARGETS lpv vct
#         DESTINATION bin/release)
#
# install(DIRECTORY ${cso_dir}
#         DESTINATION shader)
#
# install(FILES ${Assimp_BINARY_RELEASE}
#         DESTINATION bin/release)

if(NOT OPENCV_FOUND)
    set(EXCLUDE_PATTERN
        PATTERN "camera_parameters.xml" EXCLUDE
        PATTERN "demo_gi_*" EXCLUDE
        PATTERN "real_static_scene.png" EXCLUDE
        PATTERN "marker.png" EXCLUDE
        PATTERN "tracked" EXCLUDE)
endif()

# install(DIRECTORY data
#         DESTINATION .
#         PATTERN "log.txt" EXCLUDE
#         ${EXCLUDE_PATTERN})
#
# install(DIRECTORY UI
#         DESTINATION .)
