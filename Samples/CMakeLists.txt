
set(SOURCE_FILES 
	Src/BasicRenderWindow.cpp)

set(HEADER_FILES
	Include/BasicRenderWindow.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(BasicRenderWindow ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES})

set_property(TARGET BasicRenderWindow APPEND PROPERTY DEBUG_POSTFIX "_d")