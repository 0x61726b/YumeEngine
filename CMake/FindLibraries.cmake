 #

find_package(OpenGL)

if(WIN32)
      find_package(DirectX)
endif()



include_directories(
${OPENGL_INCLUDE_DIRS}
${DirectX_INCLUDE_DIRS})

link_directories(
${OPENGL_LIBRARY_DIRS}
${DirectX_LIBRARY_DIRS}
)
