set(EXECUTABLE_TARGET "AssetImporter")

set(SOURCE_FILES
AssetImporter.cc
AssimpMesh.h
AssimpMesh.cc
YumeMesh.h
YumeMesh.cc)

include_directories(${YUME_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${YUME_BOOST_PATH})
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)
include_directories(${YUME_3RDPARTY_PATH}/assimp/include)
include_directories(${DXUT_INCLUDE_DIRS})



if(MSVC)
	add_executable(${EXECUTABLE_TARGET} ${HEADER_FILES} ${SOURCE_FILES} ${COMMON_SOURCES}) #This is to avoid linker error on MSVC so tell that this is a win32 app LOL
endif()
if(NOT MSVC)
	add_executable(${EXECUTABLE_TARGET} ${HEADER_FILES} ${SOURCE_FILES} ${COMMON_SOURCES})
endif()

target_link_libraries(${EXECUTABLE_TARGET} ${YUME})
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES FOLDER "3rdParty")

source_group(${EXECUTABLE_TARGET} FILES ${HEADER_FILES} ${SOURCE_FILES})


set_output_dir(${EXECUTABLE_TARGET})
