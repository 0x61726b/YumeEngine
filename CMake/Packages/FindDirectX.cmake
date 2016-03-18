#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Find DirectX SDK
# Define:
# DirectX_FOUND
# DirectX_INCLUDE_DIR
# DirectX_LIBRARY
# DirectX_ROOT_DIR

if(WIN32) # The only platform it makes sense to check for DirectX SDK
  set (DLL_NAMES d3dcompiler_47.dll d3dcompiler_46.dll)
  if (CMAKE_CL_64)
          set (PATH_SUFFIX x64)
      else ()
          set (PATH_SUFFIX x86)
      endif ()
  find_file (DIRECT3D_DLL NAMES ${DLL_NAMES} PATHS
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0;InstallationFolder]/Redist/D3D/${PATH_SUFFIX}"
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v8.1;InstallationFolder]/Redist/D3D/${PATH_SUFFIX}"
          "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v8.0;InstallationFolder]/Redist/D3D/${PATH_SUFFIX}"
          DOC "Direct3D DLL"
          NO_DEFAULT_PATH)    # Do not use default paths such as the PATH variable, to potentially avoid using a wrong architecture DLL
  if (DIRECT3D_DLL)
                if (YUME_BUILD_DIRECT3D11)
                    set (DirectX_D3D11_LIBRARIES d3d11 d3dcompiler dxgi dxguid)
                  endif()
                unset (DIRECT3D_INCLUDE_DIRS)
            endif ()
endif(WIN32)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (DirectX REQUIRED_VARS DirectX_D3D11_LIBRARIES FAIL_MESSAGE ${FAIL_MESSAGE})
