set(SAMPLE_TARGET "HelloWorld")

set(SOURCE_FILES
Main.cc)

set(LIB_SOURCES MainDll.cc MainDll.h)

add_library(EmptyLib SHARED ${LIB_SOURCES})
add_definitions(-DBUILDING)

add_executable(EmptyProject ${SOURCE_FILES})
target_link_libraries(EmptyProject EmptyLib)

target_link_libraries(EmptyProject ${YUME})
link_boost_library(EmptyLib filesystem)
link_boost_library(EmptyLib thread)
link_boost_library(EmptyLib system)
link_boost_library(EmptyLib date_time)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${YUME_INCLUDE_DIR})
