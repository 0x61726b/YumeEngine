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
	Renderer/YumeRenderer.h
	Renderer/YumeRenderer.cc
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
	Math/YumeRect.h
	Math/YumeRect.cc
	Math/YumeColor.h
	Math/YumeColor.cc
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
	Core/YumeEnvironment.h
	Core/YumeEnvironment.cc
	Core/YumeDynamicLibrary.h
	Core/YumeDynamicLibrary.cc
	Core/YumeMain.h
	Core/YumeBase.h
	Core/YumeBase.cc
		)
set( SRC_CORE_IO
	Core/YumeIO.h
	Core/YumeIO.cc
	Core/YumeFile.h
	Core/YumeFile.cc
)
set( SRC_CORE_STL

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
)

set( SRC_ENGINE
	Engine/YumeEngine.h
	Engine/YumeEngine.cc
	Engine/YumeApplication.h
	Engine/YumeApplication.cc
)
source_group(Yume\\\\Logging FILES ${SRC_LOGGING})
source_group(Yume\\\\Math FILES ${SRC_MATH})
source_group(Yume\\\\Renderer FILES ${SRC_RENDERER})
source_group(Yume\\\\Core FILES ${SRC_CORE})
source_group(Yume\\\\Core\\\\Config FILES ${SRC_CORE_CONFIG})
source_group(Yume\\\\Core\\\\Memory FILES ${SRC_CORE_MEMORY})
source_group(Yume\\\\Core\\\\IO FILES ${SRC_CORE_IO})
source_group(Yume\\\\Core\\\\Utils FILES ${SRC_CORE_UTILS})
source_group(Yume\\\\Engine FILES ${SRC_ENGINE})


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
						)


add_definitions(-DBUILDING_YUME_ENGINE)


set(YUME_LIB_TYPE "SHARED")

target_link_libraries( ${YUME} log4cplus )
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)
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
link_boost_library(${YUME} thread)
link_boost_library(${YUME} system)
link_boost_library(${YUME} date_time)
target_link_libraries(${YUME} SDL2)
target_link_libraries(${YUME} SDL2main)
target_link_libraries(${YUME} pugixml)
target_link_libraries(${YUME} STB)
target_link_libraries(${YUME} JO)

set(YUME_LIBRARIES ${YUME_LIBRARIES} boost_filesystem boost_thread boost_system boost_date_time log4cplus)

include_directories(${Boost_INCLUDE_DIRS})

set_target_properties(${YUME} PROPERTIES FOLDER "Yume")

set(CUSTOM_TARGET_OUTPUTS ${YUME} SDL2 SDL2main)
set_output_dir( ${YUME} )
set_output_dir( SDL2 )
set_output_dir( SDL2main )

copy_assets()
