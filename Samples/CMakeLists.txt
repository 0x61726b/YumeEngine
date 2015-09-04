
set(SOURCE_FILES 
	Src/BasicRenderWindow.cpp)

set(HEADER_FILES
	Include/BasicRenderWindow.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

add_executable(BasicRenderWindow ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES} pthread) #UHH WHY DO I HAVE TO DO THIS

if(CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES} pthread) #UHH WHY DO I HAVE TO DO THIS
elseif()
	target_link_libraries(BasicRenderWindow ${YUME_LIBRARIES}) #Much better Kreygasm
endif()


set_property(TARGET BasicRenderWindow APPEND PROPERTY DEBUG_POSTFIX "_d")
