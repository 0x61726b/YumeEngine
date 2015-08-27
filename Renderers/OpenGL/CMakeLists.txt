#Configure OpenGL

set( HEADER_FILES
	)

set( SOURCE_FILES
	src/glew.cpp
	)

set(PLATFORM_HEADER_INSTALL "")
include_directories(
	BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include 
	)

include_directories(
	${OPENGL_INCLUDE_DIR}
	)

add_library(YUME_OPENGL ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(YUME_OPENGL YUME ${OPENGL_LIBRARIES})

install(FILES ${HEADER_FILES} DESTINATION include/Yume/Renderers/OpenGL)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/OpenGL DESTINATION include/Yume/Renderers/OpenGL)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/GLSL/include/ DESTINATION include/Yume/Renderers/OpenGL)
