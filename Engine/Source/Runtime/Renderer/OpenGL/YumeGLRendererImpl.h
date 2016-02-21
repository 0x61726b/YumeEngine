//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeGLRendererImpl_h__
#define __YumeGLRendererImpl_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <GLEW/glew.h>

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83f1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83f2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83f3
#endif
#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES 0x8d64
#endif
#ifndef COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8c00
#endif
#ifndef COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#define COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8c01
#endif
#ifndef COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8c02
#endif
#ifndef COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#define COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8c03
#endif

#include <SDL.h>
//----------------------------------------------------------------------------
namespace YumeEngine
{
    class YumeGraphics;

    class YumeAPIExport YumeRendererImpl
    {
        friend class YumeGraphics;
      public:
        YumeRendererImpl();

        /// Return the SDL window.
        SDL_Window* GetWindow() const { return window_; }
        /// Return the GL Context.
        const SDL_GLContext& GetGLContext() { return context_; }

      private:
              /// SDL window.
         SDL_Window* window_;
         /// SDL OpenGL context.
         SDL_GLContext context_;
         /// IOS system framebuffer handle.
         unsigned systemFBO_;
         /// Active texture unit.
         unsigned activeTexture_;
         /// Vertex attributes in use.
         unsigned enabledAttributes_;
         /// Currently bound frame buffer object.
         unsigned boundFBO_;
         /// Currently bound vertex buffer object.
         unsigned boundVBO_;
         /// Currently bound uniform buffer object.
         unsigned boundUBO_;
         /// Current pixel format.
         int pixelFormat_;
         /// Map for FBO's per resolution and format.
         
         /// Need FBO commit flag.
         bool fboDirty_;
         /// sRGB write mode flag.
         bool sRGBWrite_;

    };
}


//----------------------------------------------------------------------------
#endif
