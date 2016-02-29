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

set(RENDERER_TARGET "YumeDirect3D11")

set(SRC_RENDERER_D3D11
  YumeD3D11Required.h
	YumeD3D11ShaderVariation.h
	YumeD3D11ShaderVariation.cc
	YumeD3D11RendererImpl.h
	YumeD3D11RendererImpl.cc
	YumeD3D11ConstantBuffer.h
	YumeD3D11ConstantBuffer.cc
	YumeD3D11GpuResource.h
	YumeD3D11GpuResource.cc
	YumeD3D11Renderer.h
	YumeD3D11Renderer.cc
  YumeD3D11Shader.h
  YumeD3D11Shader.cc
  YumeD3D11VertexBuffer.h
  YumeD3D11VertexBuffer.cc
  YumeD3D11ShaderProgram.h
)

if(DirectX_FOUND)
  include_directories(${DirectX_INCLUDE_DIR})
  include_directories(${DirectX_D3D11_INCLUDE_DIR})
else()
  message( FATAL_ERROR "Direct3D 11 not found on Windows.We can't continue." )
endif()

add_definitions(-DBUILDING_YUME_DIRECT3D11)

source_group(Renderer\\\\Direct3D11 FILES ${SRC_RENDERER_D3D11})

add_library(${RENDERER_TARGET} SHARED ${SRC_RENDERER_D3D11})

target_link_libraries(${RENDERER_TARGET} ${YUME})
target_link_libraries(${RENDERER_TARGET} ${DirectX_D3D11_LIBRARIES})
target_link_libraries(${RENDERER_TARGET} FreeType)
target_link_libraries(${RENDERER_TARGET} SDL2main)
target_link_libraries(${RENDERER_TARGET} SDL2)
include_directories(${YUME_INCLUDE_DIR})
include_directories(${YUME_BOOST_PATH})
include_directories(${YUME_SDL_INCLUDE_PATH})
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)

set_output_dir(${RENDERER_TARGET})

set_folder_name(${RENDERER_TARGET} "Renderers")
