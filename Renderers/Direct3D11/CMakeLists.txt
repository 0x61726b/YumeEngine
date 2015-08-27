# Configure Direct3D11 
set( HEADER_FILES 
	Include/YumeD3D11Common.h
	Include/YumeD3D11Device.h
	)

set( SOURCE_FILES 
	Src/YumeD3D11Device.cpp
    )


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${DirectX_INCLUDE_DIR})
link_directories(${DirectX_LIBRARY_DIR})
add_definitions(-D_USRDLL)


add_library(YUME_DIRECT3D11 ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(YUME_DIRECT3D11
  YUME
  ${DirectX_LIBRARY}
)

#set_target_properties(ARKENGINE_DIRECT3D11 PROPERTIES
 #   COMPILE_DEFINITIONS OGRED3DENGINEDLL_EXPORTS
 # )

install(FILES ${HEADER_FILES} DESTINATION include/Yume/Renderers/Direct3D11)