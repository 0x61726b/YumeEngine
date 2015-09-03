# Configure Direct3D11 
set( HEADER_FILES 
	Include/YumeD3D11Required.h
	Include/YumeD3D11Device.h
	Include/YumeD3D11AdapterInfo.h
	Include/YumeD3D11AdapterInfoList.h
	Include/YumeD3D11Adapter.h
	Include/YumeD3D11AdapterList.h
	Include/YumeD3D11RenderWindow.h
	Include/YumeD3D11Renderer.h
	)

set( SOURCE_FILES 
	Src/YumeD3D11Device.cpp
	Src/YumeD3D11AdapterInfo.cpp
	Src/YumeD3D11AdapterList.cpp
	Src/YumeD3D11AdapterInfoList.cpp
	Src/YumeD3D11Adapter.cpp
	Src/YumeD3D11RenderWindow.cpp
	Src/YumeD3D11Renderer.cpp
    )


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${DirectX_INCLUDE_DIR})
link_directories(${DirectX_LIBRARY_DIR})
add_definitions(-D_USRDLL)

if(YUME_STATIC)
  set(YUME_LIB_TYPE "STATIC")
ELSE()

  set(YUME_LIB_TYPE "SHARED")
endif()
message("${YUME_LIB_TYPE}")
add_library(YUME_DIRECT3D11 ${YUME_LIB_TYPE} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(YUME_DIRECT3D11
  YUME
  ${DirectX_D3D11_LIBRARIES}
)

if (NOT YUME_STATIC)
  set_target_properties(YUME_DIRECT3D11 PROPERTIES
    COMPILE_DEFINITIONS YUME_D3D_EXPORT
  )
endif()

install(FILES ${HEADER_FILES} DESTINATION include/Yume/Renderers/Direct3D11)