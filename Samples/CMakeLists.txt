
set(SOURCE_FILES
	Src/BasicRenderWindow.cpp)

set(HEADER_FILES
	Include/BasicRenderWindow.h)

include_directories(${YUME_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Samples/Include)
include_directories(${YUME_BOOST_PATH})
include_directories(${YUME_3RDPARTY_PATH}/log4cplus/include)


if(MSVC)
  add_executable(BasicRenderWindow WIN32 ${HEADER_FILES} ${SOURCE_FILES}) #This is to avoid linker error on MSVC so tell that this is a win32 app LOL
endif()
if(NOT MSVC)
	add_executable(BasicRenderWindow ${HEADER_FILES} ${SOURCE_FILES})
endif()

target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES})

set_target_properties(BasicRenderWindow PROPERTIES FOLDER "Samples")

set_target_properties( BasicRenderWindow
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Yume"
)
#if(CMAKE_COMPILER_IS_GNUCXX)
#	target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES} pthread) #UHH WHY DO I HAVE TO DO THIS
#elseif()
	#target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES}) #Much better Kreygasm
#endif()
