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

#Setup various PATHS
set(YUME_BOOST_PATH "${CMAKE_SOURCE_DIR}/Engine/Source/3rdParty/boost")
set(YUME_ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(YUME_3RDPARTY_PATH "${CMAKE_SOURCE_DIR}/Engine/Source/3rdParty")
SET(YUME_TEMPLATES_DIR "${CMAKE_SOURCE_DIR}/CMake/Templates")
set(YUME_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Engine/Source/Runtime")
set(YUME_INCLUDE_DIR ${YUME_INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/Engine/Source/Runtime")
