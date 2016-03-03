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
#include "YumeD3D11Required.h"
#include "Renderer/YumeTexture2D.h"

#include "YumeD3D11GpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3DExport YumeD3D11Texture2D : public YumeTexture2D,public YumeD3D11Resource
	{
	public:
		YumeD3D11Texture2D(YumeRHI* rhi);
		virtual ~YumeD3D11Texture2D();

		virtual void Release();

		/// Set size, format and usage. Zero size will follow application window size. Return true if successful.
		virtual bool SetSize(int width,int height,unsigned format,TextureUsage usage = TEXTURE_STATIC);
		/// Set data either partially or fully on a mip level. Return true if successful.
		virtual bool SetData(unsigned level,int x,int y,int width,int height,const void* data);
		/// Set data from an image. Return true if successful. Optionally make a single channel image alpha-only.
		virtual bool SetData(SharedPtr<YumeImage> image,bool useAlpha = false);

		virtual bool GetData(unsigned level,void* dest) const;

		/// Create texture.
		virtual bool Create();
		/// Handle render surface update event.
		virtual void HandleRenderSurfaceUpdate(YumeHash eventType,VariantMap& eventData);

		virtual unsigned GetRowDataSize(int width) const;

		virtual void UpdateParameters();

		virtual void CheckTextureBudget(YumeHash type);

		virtual unsigned GetSRVFormat(unsigned format);
		/// Return the depth-stencil view format corresponding to a texture format. Handles conversion of typeless depth texture formats.
		virtual unsigned GetDSVFormat(unsigned format);
		/// Convert format to sRGB.
		virtual unsigned GetSRGBFormat(unsigned format);

		virtual bool IsCompressed() const;
	};
}


//----------------------------------------------------------------------------
#endif
