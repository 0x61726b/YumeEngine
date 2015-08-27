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
	${CMAKE_BINARY_DIR}/include/YumeBuildSettings.h)

set( SOURCE_FILES 
	Src/Pch.cpp )

add_definitions(-D_MT -D_USRDLL)
set (TARGET_LINK_FLAGS "")

if(YUME_STATIC)
	ADD_LIBRARY(YUME STATIC ${SOURCE_FILES} ${HEADER_FILES})
ELSE()
	ADD_LIBRARY(YUME SHARED ${SOURCE_FILES} ${HEADER_FILES})
endif()

if(WIN32 AND MSVC10 AND CMAKE_CL_64)
  set_target_properties(YUME PROPERTIES 
                        VERSION 1
                        LINK_FLAGS "/INCREMENTAL:NO"
                      )
else()
  set_target_properties(YUME PROPERTIES VERSION 1)
endif()

use_precompiled_header(YUME 
  "${CMAKE_CURRENT_SOURCE_DIR}/include/YumeHeaders.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Pch.cpp"
)

install(FILES ${HEADER_FILES}
  DESTINATION include/Yume
)