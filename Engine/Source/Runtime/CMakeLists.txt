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
	Renderer/YumeRendererDefs.cc
	Renderer/YumeGraphicsApi.h
	Renderer/YumeRHI.h
	Renderer/YumeRHI.cc
	Renderer/YumeRendererImpl.h
	Renderer/YumeGpuResource.h
	Renderer/YumeGpuResource.cc
	Renderer/YumeImage.h
	Renderer/YumeImage.cc
	Renderer/YumeResource.h
	Renderer/YumeResource.cc
	Renderer/YumeBackgroundWorker.h
	Renderer/YumeBackgroundWorker.cc
	Renderer/YumeResourceManager.h
	Renderer/YumeResourceManager.cc
	Renderer/YumeShaderVariation.h
	Renderer/YumeShaderVariation.cc
	Renderer/YumeShader.h
	Renderer/YumeShader.cc
	Renderer/YumeVertexBuffer.h
	Renderer/YumeVertexBuffer.cc
	Renderer/YumeIndexBuffer.h
	Renderer/YumeIndexBuffer.cc
	Renderer/YumeConstantBuffer.h
	Renderer/YumeConstantBuffer.cc
	Renderer/YumeInputLayout.h
	Renderer/YumeInputLayout.cc
	Renderer/YumeTexture.h
	Renderer/YumeTexture.cc
	Renderer/YumeTexture2D.h
	Renderer/YumeTexture2D.cc
	Renderer/YumeRenderable.h
	Renderer/YumeRenderable.cc
	Renderer/YumeGeometry.h
	Renderer/YumeGeometry.cc
	Renderer/YumeTexture3D.h
	Renderer/YumeTexture3D.cc
	Renderer/YumeTextureCube.h
	Renderer/YumeTextureCube.cc
	Renderer/LPVRendererTest.h
	Renderer/LPVRendererTest.cc
	Renderer/LightPropagationVolume.h
	Renderer/LightPropagationVolume.cc
	Renderer/AssimpMesh.h
	Renderer/AssimpMesh.cc
	Renderer/YumeMesh.h
	Renderer/YumeMesh.cc
	Renderer/YumeCompositeMesh.h
	Renderer/YumeCompositeMesh.cc
	Renderer/YumeLPVCamera.h
	Renderer/YumeLPVCamera.cc
	Renderer/YumeSkydome.h
	Renderer/YumeSkydome.cc
	Renderer/YumeMiscRenderer.h
	Renderer/YumeMiscRenderer.cc
	Renderer/YumePostProcess.h
	Renderer/YumePostProcess.cc
	Renderer/RenderCall.h
	Renderer/RenderCall.cc
	Renderer/RenderPass.h
	Renderer/RenderPass.cc
	Renderer/Light.h
	Renderer/Light.cc
	Renderer/SceneNode.h
	Renderer/SceneNode.cc
	Renderer/Scene.h
	Renderer/Scene.cc
	Renderer/StaticModel.h
	Renderer/StaticModel.cc
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
	Math/YumeMatrix3x4.h
	Math/YumeMatrix3x4.cc
	Math/YumeQuaternion.h
	Math/YumeQuaternion.cc
	Math/YumePlane.h
	Math/YumePlane.cc
	Math/YumeSphere.h
	Math/YumeSphere.cc
	Math/YumePolyhedron.h
	Math/YumePolyhedron.cc
	Math/YumeBoundingBox.h
	Math/YumeBoundingBox.cc
	Math/YumeFrustum.h
	Math/YumeFrustum.cc
	Math/YumeRay.h
	Math/YumeRay.cc
	Math/YumeMath.h
	Math/YumeMath.cc
	Math/YumeRect.h
	Math/YumeRect.cc
	Math/YumeColor.h
	Math/YumeColor.cc
	Math/YumeHash.h
	Math/YumeHash.cc
)

set( SRC_CORE
	Core/Pch.cc
	Core/YumeCommon.cc
	Core/YumeHeaders.h
  Core/YumeRequired.h
	Core/YumeRequired.cc
  Core/Yume.h
  Core/YumeCommon.h
  Core/YumeStdHeaders.h
	Core/PlatformImpl.h
	Core/PlatformImpl_Win32.cc
	Core/YumeHeaderSuffix.h
	Core/YumeHeaderPrefix.h
	Core/YumeEnvironment.h
	Core/YumeEnvironment.cc
	Core/YumeDynamicLibrary.h
	Core/YumeDynamicLibrary.cc
	Core/YumeMain.h
	Core/YumeBase.h
	Core/YumeBase.cc
	Core/YumeWorkQueue.h
	Core/YumeWorkQueue.cc
	Core/YumeComponentAnimation.h
	Core/YumeComponentAnimation.cc
	Core/YumeComponentAnimInfo.h
	Core/YumeComponentAnimInfo.cc
	Core/YumeVectorBuffer.h
	Core/YumeVectorBuffer.cc
	Core/YumeXmlFile.h
	Core/YumeXmlFile.cc
	Core/YumeJsFile.h
	Core/YumeJsFile.cc
		)
set( SRC_CORE_IO
	Core/YumeIO.h
	Core/YumeIO.cc
	Core/YumeFile.h
	Core/YumeFile.cc
)
set( SRC_INPUT
	Input/YumeInput.h
	Input/YumeInput.cc
)
set( SRC_CORE_CONFIG
	Core/YumeConfig.h
	Core/YumePlatform.h
)
set( SRC_CORE_UTILS
	Core/YumeXmlParser.h
	Core/YumeXmlParser.cc
	Core/YumeDefaults.h
	Core/YumeDefaults.cc
	Core/YumeTimer.h
	Core/YumeTimer.cc
	Core/YumeVariant.h
	Core/YumeVariant.cc
	Core/YumeThread.h
	Core/YumeThread.cc
	Core/YumeDecompresser.h
	Core/YumeDecompresser.cc
	Core/YumeEventHub.h
	Core/YumeSortAlgorithms.h
	Core/YumeStreamReader.h
	Core/YumeStreamReader.cc
	Core/YumeStreamWriter.h
	Core/YumeStreamWriter.cc
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
	Core/YumeAlignedAllocator.cc
	Core/YumeBitPattern.h
	Core/RefCounter.h
	Core/RefCounter.cc
	Core/SharedPtr.h
	Core/YumeMutex.h
	Core/YumeMutex.cc
	Core/YumeAlloc.h
	Core/YumeAlloc.cc
)

set( SRC_ENGINE
	Engine/YumeEngine.h
	Engine/YumeEngine.cc
	Engine/YumeApplication.h
	Engine/YumeApplication.cc
)

set(SRC_UI
UI/YumeUI.h
UI/YumeUI.cc
UI/YumeDebugOverlay.h
UI/YumeDebugOverlay.cc
UI/YumeUIBinding.h
UI/YumeUIBinding.cc
UI/YumeUIElement.h
UI/YumeUIElement.cc)

file(GLOB CONTAINER_SOURCES "Container/*.cpp" "Container/*.h")

source_group(Yume\\\\Logging FILES ${SRC_LOGGING})
source_group(Yume\\\\Math FILES ${SRC_MATH})
source_group(Yume\\\\Renderer FILES ${SRC_RENDERER})
source_group(Yume\\\\Core FILES ${SRC_CORE})
source_group(Yume\\\\Core\\\\Config FILES ${SRC_CORE_CONFIG})
source_group(Yume\\\\Core\\\\Memory FILES ${SRC_CORE_MEMORY})
source_group(Yume\\\\Core\\\\IO FILES ${SRC_CORE_IO})
source_group(Yume\\\\Core\\\\Utils FILES ${SRC_CORE_UTILS})
source_group(Yume\\\\Engine FILES ${SRC_ENGINE})
source_group(Yume\\\\Scene FILES ${SRC_SCENE})
source_group(Yume\\\\Input FILES ${SRC_INPUT})
source_group(Yume\\\\Container FILES ${CONTAINER_SOURCES})
source_group(Yume\\\\UI FILES ${SRC_UI})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${YUME_3RDPARTY_PATH})
include_directories(${YUME_SDL_INCLUDE_PATH})

if(OS_LINUX OR OS_MACOSX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBOOST_ALL_NO_LIB=1 -g -O0 --coverage")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if(YUME_TEST_MODE)
	add_definitions(-DYUME_TEST_MODE)
endif()
ADD_LIBRARY(${YUME} SHARED
						${SRC_LOGGING}
 						${SRC_MATH}
					  ${SRC_CORE}
						${SRC_GRAPHICS}
					  ${SRC_CORE_MEMORY}
					  ${SRC_CORE_CONFIG}
						${SRC_CORE_UTILS}
						${SRC_CORE_IO}
						${SRC_RENDERER}
						${SRC_ENGINE}
						${SRC_INPUT}
						${CONTAINER_SOURCES}
						${SRC_UI}
						)


add_definitions(-DBUILDING_YUME_ENGINE)


set(YUME_LIB_TYPE "SHARED")

target_link_libraries( ${YUME} log4cplus )
target_link_libraries( ${YUME} ${DXUT_LIBRARIES} )
target_link_libraries( ${YUME} ${D3D_LIBS} )
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)
include_directories(${YUME_3RDPARTY_PATH}/assimp/include)
include_directories(${DXUT_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR}/Engine/Source/3rdParty/log4cplus/include)

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
link_boost_library(${YUME} system)
link_boost_library(${YUME} date_time)
target_link_libraries(${YUME} boost_system)
target_link_libraries(${YUME} SDL2)
target_link_libraries(${YUME} pugixml)
target_link_libraries(${YUME} STB)
target_link_libraries(${YUME} JO)
target_link_libraries(${YUME} assimp)
target_link_libraries(${YUME} Cef3D
)
set(YUME_LIBRARIES ${YUME_LIBRARIES} boost_filesystem boost_system boost_date_time log4cplus)

include_directories(${YUME_INCLUDE_DIR})

set_target_properties(${YUME} PROPERTIES FOLDER "Yume")

set(CUSTOM_TARGET_OUTPUTS ${YUME} SDL2 SDL2main)
set_output_dir( ${YUME} )
set_output_dir( SDL2 )
set_output_dir( SDL2main )

copy_assets()
