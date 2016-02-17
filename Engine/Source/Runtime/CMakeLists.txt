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

set(SRC_GRAPHICS
	Graphics/YumeRenderer.cc
	Graphics/YumeRenderTarget.cc
	Graphics/YumeRenderWindow.cc
	Graphics/YumeRendererCapabilities.cc
	Graphics/YumeViewport.cc
	Graphics/YumeViewport.h
	Graphics/YumeRenderer.h
	Graphics/YumeRenderTarget.h
	Graphics/YumeRenderWindow.h
	Graphics/YumeRendererCapabilities.h
)
set( SRC_LOGGING
		Logging/logging.h
		Logging/log4cplus.cc
 )

set( SRC_MATH
	Math/asm_math.h
	Math/YumeVector2.cc
	Math/YumeVector3.cc
	Math/YumeVector4.cc
	Math/YumeMatrix3.cc
	Math/YumeMatrix4.cc
	Math/YumeQuaternion.cc
	Math/YumePlane.cc
	Math/YumeMath.cc
	Math/YumeAxisAlignedBox.cc
	Math/YumeMath.h
	Math/YumeVector2.h
	Math/YumeVector3.h
	Math/YumeVector4.h
	Math/YumeMatrix3.h
	Math/YumeMatrix4.h
	Math/YumePlaneBoundedVolume.h
	Math/YumeQuaternion.h
	Math/YumePlane.h
	Math/YumeSphere.h
	Math/YumeRay.h
	Math/YumeAxisAlignedBox.h
)

set( SRC_CORE
	Core/Pch.cc
	Core/YumeCommon.cc
	Core/YumeCentrum.cc
	Core/YumeHeaders.h
  Core/YumeRequired.h
  Core/Yume.h
  Core/YumeCentrum.h
  Core/YumeCommon.h
  Core/YumeStdHeaders.h
	Core/YumeString.h
	Core/PlatformImpl.h
	Core/PlatformImpl_Win32.cc
		)
set( SRC_CORE_CONFIG
	Core/YumeConfig.h
	Core/YumePlatform.h
	${CMAKE_BINARY_DIR}/Include/YumeBuildSettings.h
)
set( SRC_CORE_UTILS
	Core/YumeWindowMessageListener.cc
	Core/YumeString.cc
	Core/YumeStringConverter.cc
	Core/YumeStringVector.h
	Core/YumeStringConverter.h
	Core/YumeConfigOption.h
	Core/YumeWindowMessageListener.h
	Core/YumeHeaderSuffix.h
	Core/YumeHeaderPrefix.h
)
set( SRC_CORE_MEMORY
	Core/YumeMallocObject.h
  Core/YumeMemoryAllocatorNed.h
  Core/YumeMemoryAllocatorNedPooling.h
  Core/YumeMemoryAllocatorSTL.h
  Core/YumeMemoryAllocatorConfig.h
	Core/YumeMemoryAllocatorNed.cc
	Core/YumeMallocObject.cc
	Core/YumeMemoryAllocatorNedPooling.cc
)
source_group(Yume\\\\Logging FILES ${SRC_LOGGING})
source_group(Yume\\\\Math FILES ${SRC_MATH})
source_group(Yume\\\\Graphics FILES ${SRC_GRAPHICS})
source_group(Yume\\\\Core FILES ${SRC_CORE})
source_group(Yume\\\\Core\\\\Memory FILES ${SRC_CORE_MEMORY})
source_group(Yume\\\\Core\\\\Utils FILES ${SRC_CORE_UTILS})
source_group(Yume\\\\Core\\\\Config FILES ${SRC_CORE_CONFIG})



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core/nedmalloc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_BINARY_DIR})



ADD_LIBRARY(${YUME} SHARED
						${SRC_LOGGING}
 						${SRC_MATH}
					  ${SRC_CORE}
						${SRC_GRAPHICS}
					  ${SRC_CORE_MEMORY}
					  ${SRC_CORE_UTILS}
					  ${SRC_CORE_CONFIG}
						)

add_definitions(-DBUILDING_YUME_ENGINE)

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

set(YUME_LIBRARIES ${YUME_LIBRARIES} boost_filesystem boost_thread boost_system boost_date_time log4cplus)

include_directories(${Boost_INCLUDE_DIRS})

set_target_properties(${YUME} PROPERTIES FOLDER "Yume")
set_target_properties(log4cplus PROPERTIES FOLDER "3rdParty")

set_target_properties( ${YUME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
)
