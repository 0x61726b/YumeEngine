set(SAMPLE_TARGET "CefTest")

set(SOURCE_FILES CefTest.cpp)

set(HEADER_FILES CefTest.h)

include_directories(${YUME_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${YUME_BOOST_PATH})
include_directories(${CEF_INCLUDE_DIRS})
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)


set(COMMON_SOURCES
	${CMAKE_SOURCE_DIR}/Samples/Common/Common.h
	${CMAKE_SOURCE_DIR}/Samples/Common/Common.cc)

if(MSVC)
	add_executable(${SAMPLE_TARGET} WIN32 ${HEADER_FILES} ${SOURCE_FILES} ${COMMON_SOURCES}) #This is to avoid linker error on MSVC so tell that this is a win32 app LOL
endif()
if(NOT MSVC)
	add_executable(${SAMPLE_TARGET} ${HEADER_FILES} ${SOURCE_FILES} ${COMMON_SOURCES})
endif()

target_link_libraries(${SAMPLE_TARGET} ${YUME})
set_target_properties(${SAMPLE_TARGET} PROPERTIES FOLDER "Samples")

source_group(${SAMPLE_TARGET} FILES ${HEADER_FILES} ${SOURCE_FILES})
source_group(Common FILES ${COMMON_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/Samples)

set_output_dir(${SAMPLE_TARGET})

target_link_libraries(${SAMPLE_TARGET} SDL2main)
