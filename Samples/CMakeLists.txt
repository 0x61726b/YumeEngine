
set(SOURCE_FILES 
	Src/BasicRenderWindow.cpp)

set(HEADER_FILES
	Include/BasicRenderWindow.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)



if(MSVC)
  add_executable(BasicRenderWindow WIN32 ${HEADER_FILES} ${SOURCE_FILES}) #This is to avoid linker error on MSVC so tell that this is a win32 app LOL
endif()
if(NOT MSVC)
	add_executable(BasicRenderWindow ${HEADER_FILES} ${SOURCE_FILES})
endif()

target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES})

#if(CMAKE_COMPILER_IS_GNUCXX)
#	target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES} pthread) #UHH WHY DO I HAVE TO DO THIS
#elseif()
	#target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES}) #Much better Kreygasm
#endif()


set_property(TARGET BasicRenderWindow APPEND PROPERTY DEBUG_POSTFIX "_d")
