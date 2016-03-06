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
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeD3D11Texture2D_h__
#define __YumeD3D11Texture2D_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"
#include "Renderer/YumeTexture2D.h"

#include "YumeGLGpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGLExport YumeGLTexture2D : public YumeTexture2D,public YumeGLResource
	{
	public:
		YumeGLTexture2D(YumeRHI* rhi);
		virtual ~YumeGLTexture2D();

		virtual void Release();

		virtual void OnDeviceLost();
		virtual void OnDeviceReset();

		/// Set size, format and usage. Zero size will follow application window size. Return true if successful.
		virtual bool SetSize(int width,int height,unsigned format,TextureUsage usage = TEXTURE_STATIC);
		/// Set data either partially or fully on a mip level. Return true if successful.
		virtual bool SetData(unsigned level,int x,int y,int width,int height,const void* data);
		/// Set data from an image. Return true if successful. Optionally make a single channel image alpha-only.
		virtual bool SetData(SharedPtr<YumeImage> image,bool useAlpha = false);

		virtual bool GetData(unsigned level,void* dest) const;

		static unsigned GetDataType(unsigned format);

		static unsigned GetExternalFormat(unsigned format);

		/// Create texture.
		virtual bool Create();
		/// Handle render surface update event.
		virtual void HandleRenderSurfaceUpdate(YumeHash eventType,VariantMap& eventData);

		virtual unsigned GetRowDataSize(int width) const;

		virtual void UpdateParameters();

		virtual void CheckTextureBudget(YumeHash type);

		virtual unsigned GetDataSize(int width,int height) const;

		virtual bool IsCompressed() const;

		const unsigned GetTarget() { return target_; }

		unsigned GetSRVFormat(unsigned format) { return 0; }
		/// Return the depth-stencil view format corresponding to a texture format. Handles conversion of typeless depth texture formats.
		unsigned GetDSVFormat(unsigned format) { return 0; }
		/// Convert format to sRGB.
		unsigned GetSRGBFormat(unsigned format) { return 0; }
	};
}


//----------------------------------------------------------------------------
#endif
