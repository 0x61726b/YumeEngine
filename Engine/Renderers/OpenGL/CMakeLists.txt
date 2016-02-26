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
# Date : 2.21.2016
# Comments :
################################################################################
set(RENDERER_TARGET "YumeOpenGL")

set(SRC_RENDERER_GL
	YumeGLRendererImpl.h
	YumeGLRendererImpl.cc
	YumeGLGpuResource.h
	YumeGLGpuResource.cc
	YumeGLRenderer.h
	YumeGLRenderer.cc
  YumeGLRequired.h
)

if(OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIR})
else()
  message( FATAL_ERROR "Direct3D 11 not found on Windows.We can't continue." )
endif()

add_definitions(-DBUILDING_YUME_OPENGL)

source_group(Renderer\\\\OpenGL FILES ${SRC_RENDERER_GL})

add_coverage()

add_library(${RENDERER_TARGET} SHARED ${SRC_RENDERER_GL})

target_link_libraries(${RENDERER_TARGET} ${YUME})
target_link_libraries(${RENDERER_TARGET} ${OPENGL_LIBRARIES})
target_link_libraries(${RENDERER_TARGET} FreeType)
target_link_libraries(${RENDERER_TARGET} GLEW)
include_directories(${YUME_INCLUDE_DIR})

add_definitions(-DGLEW_NO_GLU)
add_definitions(-DGLEW_STATIC)

set_output_dir(${RENDERER_TARGET})

set_folder_name(${RENDERER_TARGET} "Renderers")
