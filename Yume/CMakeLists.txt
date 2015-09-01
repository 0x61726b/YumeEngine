#
#
#
PROJECT(YUME)
include(PrecompiledHeader)


set( HEADER_FILES 
	Include/YumeConfig.h
	Include/YumeHeaders.h
	Include/YumePlatform.h
  Include/YumeRequired.h
  Include/Yume.h
  Include/YumeCommon.h
  Include/YumeHeaderSuffix.h
  Include/YumeStdHeaders.h
  Include/YumeHeaderPrefix.h
  Include/YumeMallocObject.h
  Include/YumeMemoryAllocatorNed.h
  Include/YumeMemoryAllocatorNedPooling.h
  Include/YumeMemoryAllocatorSTL.h
  Include/YumeMemoryAllocatorConfig.h
  Include/YumeRenderer.h
  Include/YumeRenderTarget.h
  Include/YumeRenderWindow.h
	${CMAKE_BINARY_DIR}/Include/YumeBuildSettings.h)

set( SOURCE_FILES 
	Src/Pch.cpp
  Src/YumeMemoryAllocatorNed.cpp
  Src/YumeMemoryAllocatorNedPooling.cpp
  Src/YumeCommon.cpp
  Src/YumeRenderer.cpp
  Src/YumeRenderTarget.cpp
  Src/YumeRenderWindow.cpp )



add_definitions(-D_MT -D_USRDLL -DYUME_NONCLIENT_BUILD)
set (TARGET_LINK_FLAGS "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Src/nedmalloc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})


if(YUME_STATIC)
	ADD_LIBRARY(YUME STATIC ${SOURCE_FILES} ${HEADER_FILES})
ELSE()
	ADD_LIBRARY(YUME SHARED ${SOURCE_FILES} ${HEADER_FILES})
endif()

set(YUME_STATIC_LIB YUME_STATIC)
if(WIN32 AND MSVC10 AND CMAKE_CL_64)
  set_target_properties(YUME PROPERTIES 
                        VERSION 1
                        LINK_FLAGS "/INCREMENTAL:NO"
                      )
else()
  set_target_properties(YUME PROPERTIES VERSION 1)
endif()

use_precompiled_header(YUME 
  "${CMAKE_CURRENT_SOURCE_DIR}/Include/YumeHeaders.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Src/Pch.cpp"
)


install(FILES ${HEADER_FILES}
  DESTINATION Include/Yume
)
