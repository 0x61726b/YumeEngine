


#Direct3D11
if(YUME_BUILD_DIRECT3D11)
	if(DirectX_D3D11_FOUND)
		add_subdirectory(Direct3D11)
	endif()
endif()


#OpenGL
if(YUME_BUILD_OPENGL)
	add_subdirectory(OpenGL)
endif()

#Direct3D 12
if(YUME_BUILD_DIRECT3D12)
	if(DirectX_D3D12_FOUND)
		add_subdirectory(Direct3D12)
	endif()
endif()