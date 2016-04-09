


set(SRC
  pppipe.cpp
  pppipe.h
  common_gui.cpp
  common.h
  common_gui.h common_renderer.cpp common_renderer.h common_dxut.cpp common_dxut.h)

include_directories(
      ${Boost_INCLUDE_DIR}
      ${D3D_INCLUDE_DIR}
      ${WINDOWS_SDK_INCLUDE_DIR}
      ${DXUT_INCLUDE_DIRS}
      ${Assimp_INCLUDE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}
  		${YUME_3RDPARTY_PATH}
  		${YUME_3RDPARTY_PATH}/assimp/include
    ${YUME_INCLUDE_DIR})


add_executable(LightPropogateVolumes WIN32 main_gi.cpp skydome.h ${SRC})
set_target_properties (LightPropogateVolumes PROPERTIES COMPILE_DEFINITIONS "LPV")
target_link_libraries(LightPropogateVolumes
      YumeEngine
      ${D3D_LIBS}
      assimp
      comctl32.lib)
