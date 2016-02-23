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

set(RENDERER_TARGET "YumeNull")

set(SRC_RENDERER_NULL
	YumeNullRenderer.h
	YumeNullRenderer.cc
)

add_definitions(-DBUILDING_YUME_NULL)

source_group(Renderer\\\\Null FILES ${SRC_RENDERER_NULL})

add_library(${RENDERER_TARGET} SHARED ${SRC_RENDERER_NULL})

target_link_libraries(${RENDERER_TARGET} ${YUME})
include_directories(${YUME_INCLUDE_DIR})
include_directories(${YUME_BOOST_PATH})
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)

set_output_dir_to_yume(${RENDERER_TARGET})

set_folder_name(${RENDERER_TARGET} "Renderers")
