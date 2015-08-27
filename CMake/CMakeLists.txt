#
if(WIN32 OR APPLE)
  set(YUME_CMAKE_DIR "CMake")
else()
	set(YUME_CMAKE_DIR "${YUME_LIB_DIRECTORY}/Yume/Cmake")
endif()
#
set(INST_FILES
  Utils/FindPkgMacros.cmake
  Utils/PrecompiledHeader.cmake
) 
set(STATIC_INST_FILES
  Packages/FindDirectX.cmake

)
if (WIN32)
  set(INST_FILES ${INST_FILES}
    Templates/VisualStudioUserFile.vcproj.user.in
    Templates/VisualStudioUserFile.vcxproj.user.in
  )
endif ()



install(FILES ${INST_FILES} DESTINATION ${YUME_CMAKE_DIR})

if(YUME_STATIC)
	install(FILES ${STATIC_INST_FILES} DESTINATION ${YUME_CMAKE_DIR})	
endif()
