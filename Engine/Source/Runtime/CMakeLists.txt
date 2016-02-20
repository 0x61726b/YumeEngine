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
PROJECT(Yume)
include(PrecompiledHeader)

set(SRC_RENDERER
	Renderer/YumeRendererDefs.h
	Renderer/YumeGraphicsApi.h
)
set(SRC_RENDERER_D3D11
	Renderer/Direct3D11/YumeD3D11ShaderVariation.h
	Renderer/Direct3D11/YumeD3D11ShaderVariation.cc
	Renderer/Direct3D11/YumeD3D11RendererImpl.h
	Renderer/Direct3D11/YumeD3D11RendererImpl.cc
	Renderer/Direct3D11/YumeD3D11ConstantBuffer.h
	Renderer/Direct3D11/YumeD3D11ConstantBuffer.cc
	Renderer/Direct3D11/YumeD3D11GpuResource.h
	Renderer/Direct3D11/YumeD3D11GpuResource.cc
	Renderer/Direct3D11/YumeD3D11Graphics.h
	Renderer/Direct3D11/YumeD3D11Graphics.cc
)
set( SRC_LOGGING
		Logging/logging.h
		Logging/log4cplus.cc
 )

set( SRC_MATH
	Math/asm_math.h
	Math/YumeVector2.h
	Math/YumeVector2.cc
	Math/YumeVector3.h
	Math/YumeVector3.cc
	Math/YumeVector4.h
	Math/YumeVector4.cc
	Math/YumeMatrix3.h
	Math/YumeMatrix3.cc
	Math/YumeMatrix4.h
	Math/YumeMatrix4.cc
	Math/YumeQuaternion.h
	Math/YumeQuaternion.cc
	Math/YumePlane.h
	Math/YumePlaneBoundedVolume.h
	Math/YumePlane.cc
	Math/YumeSphere.h
	Math/YumeRay.h
	Math/YumeAxisAlignedBox.h
	Math/YumeMath.h
	Math/YumeMath.cc
)

set( SRC_CORE
	Core/Pch.cc
	Core/YumeCommon.cc
	Core/YumeHeaders.h
  Core/YumeRequired.h
  Core/Yume.h
  Core/YumeCommon.h
  Core/YumeStdHeaders.h
	Core/PlatformImpl.h
	Core/PlatformImpl_Win32.cc
	Core/YumeHeaderSuffix.h
	Core/YumeHeaderPrefix.h
		)
set( SRC_CORE_STL

)
set( SRC_CORE_CONFIG
	Core/YumeConfig.h
	Core/YumePlatform.h
)
set( SRC_CORE_UTILS

)
set( SRC_CORE_MEMORY
	Core/YumeMemoryAllocatorSTL.h
	Core/YumeMemoryAllocatorConfig.h
	Core/YumeMemoryAllocatorNed.h
	Core/YumeMemoryAllocatorNedPooling.h
	Core/YumeMemoryAllocatorNed.cc
	Core/YumeMemoryAllocatorNedPooling.cc
	Core/YumeMallocObject.h
	Core/YumeMallocObject.cc
	Core/YumeMemoryAllocatorStd.h
	Core/YumeAlignedAllocator.h
)

set( SRC_ENGINE
	Engine/YumeEngine.h
	Engine/YumeEngine.cc
)
source_group(Yume\\\\Logging FILES ${SRC_LOGGING})
source_group(Yume\\\\Math FILES ${SRC_MATH})
source_group(Yume\\\\Renderer FILES ${SRC_RENDERER})
source_group(Yume\\\\Renderer\\\\Direct3D11 FILES ${SRC_RENDERER_D3D11})
source_group(Yume\\\\Core FILES ${SRC_CORE})
source_group(Yume\\\\Core\\\\Config FILES ${SRC_CORE_CONFIG})
source_group(Yume\\\\Core\\\\Memory FILES ${SRC_CORE_MEMORY})
source_group(Yume\\\\Engine FILES ${SRC_ENGINE})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${YUME_3RDPARTY_PATH})
include_directories(${YUME_SDL_INCLUDE_PATH})
include_directories(${CMAKE_BINARY_DIR}/Engine/3rdParty/log4cplus/include)




ADD_LIBRARY(${YUME} STATIC
						${SRC_LOGGING}
 						${SRC_MATH}
					  ${SRC_CORE}
						${SRC_GRAPHICS}
					  ${SRC_CORE_MEMORY}
					  ${SRC_CORE_UTILS}
					  ${SRC_CORE_CONFIG}
						${SRC_CORE_STL}
						${SRC_RENDERER_D3D11}
						${SRC_RENDERER}
						${SRC_ENGINE}
						)

add_definitions(-DBUILDING_YUME_ENGINE)
add_definitions(-DYUME_DIRECT3D11)

set(YUME_LIB_TYPE "SHARED")

target_link_libraries( ${YUME} log4cplus )
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)


if(WIN32 AND MSVC10 AND CMAKE_CL_64)
  set_target_properties(${YUME} PROPERTIES
                        VERSION 1
                        LINK_FLAGS "/INCREMENTAL:NO"
                      )
else()
  set_target_properties(${YUME} PROPERTIES VERSION 1)
endif()

if(YUME_USE_PCH)
	use_precompiled_header(${YUME}
	  "${CMAKE_CURRENT_SOURCE_DIR}/Core/YumeHeaders.h"
	  "${CMAKE_CURRENT_SOURCE_DIR}/Core/Pch.cc"
	)
endif()



link_boost_library(${YUME} filesystem)
link_boost_library(${YUME} thread)
link_boost_library(${YUME} system)
link_boost_library(${YUME} date_time)
target_link_libraries(${YUME} SDL2-static)
target_link_libraries(${YUME} FreeType)

set(YUME_LIBRARIES ${YUME_LIBRARIES} boost_filesystem boost_thread boost_system boost_date_time log4cplus)

include_directories(${Boost_INCLUDE_DIRS})

set_target_properties(${YUME} PROPERTIES FOLDER "Yume")


set_target_properties( ${YUME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
)
