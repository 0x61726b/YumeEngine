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
# File : CmakeLists.txt
# Date : 8.27.2015
# Comments :
################################################################################


# Define source files
set(SOURCE_FILES
  stb_image.h
  stb_image.c
  stb_image_write.h
  stb_image_write.c
  stb_rect_pack.h
  stb_vorbis.c
  stb_vorbis.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# Setup target
ADD_LIBRARY(STB ${SOURCE_FILES} )
