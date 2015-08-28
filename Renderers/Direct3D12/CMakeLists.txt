# Configure Direct3D12 
set( HEADER_FILES 
	Include/YumeD3D12Common.h
	Include/YumeD3D12Device.h
	)

set( SOURCE_FILES 
	Src/YumeD3D12Device.cpp
    )


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${DirectX_D3D12_INCLUDE_DIR})
link_directories(${DirectX_D3D12_LIBRARY})
add_definitions(-D_USRDLL)


add_library(YUME_DIRECT3D12 ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(YUME_DIRECT3D12
  YUME
  ${DirectX_D3D12_LIBRARIES}
)


install(FILES ${HEADER_FILES} DESTINATION include/Yume/Renderers/Direct3D12)