#############################
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

configure_file(${YUME_TEMPLATES_DIR}/CMakeBuildSettings.h.in ${CMAKE_CURRENT_BINARY_DIR}/Include/YumeBuildSettings.h @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Include/YumeBuildSettings.h DESTINATION include/Yume)
