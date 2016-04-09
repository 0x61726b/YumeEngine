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
set( SRC_LOGGING
		Logging/logging.h
		Logging/log4cplus.cc
 )

set( SRC_ENGINE
	Engine/YumeEngine.h
	Engine/YumeEngine.cc
	Engine/YumeApplication.h
	Engine/YumeApplication.cc
)

file(GLOB RendererSrc "Renderer/*.cc" "Renderer/*.h")

source_group(Yume\\\\Logging FILES ${SRC_LOGGING})
source_group(Yume\\\\Renderer FILES ${RendererSrc})

include_directories(
    ${Boost_INCLUDE_DIR}
    ${D3D_INCLUDE_DIR}
    ${WINDOWS_SDK_INCLUDE_DIR}
    ${DXUT_INCLUDE_DIRS}
    ${Assimp_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
		${YUME_3RDPARTY_PATH}
		${YUME_3RDPARTY_PATH}/assimp/include)

set(YUME "YumeEngine")

ADD_LIBRARY(${YUME} STATIC
						${RendererSrc}
						)


add_definitions(-DBUILDING_YUME_ENGINE)



target_link_libraries(${YUME} log4cplus )
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)
include_directories(${CMAKE_BINARY_DIR}/Engine/Source/3rdParty/log4cplus/include)

target_link_libraries(${YUME} pugixml)
