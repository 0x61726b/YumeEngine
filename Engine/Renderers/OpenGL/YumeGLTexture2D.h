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
#include "Core/YumeEventHub.h"
#include "YumeGLGpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGLExport YumeGLTexture2D : public YumeTexture2D,public YumeGLResource,public RendererEventListener
	{
	public:
		YumeGLTexture2D();
		virtual ~YumeGLTexture2D();

		virtual void Release();
		virtual void OnDeviceLost();
		virtual void OnDeviceReset();

		virtual bool SetSize(int width,int height,unsigned format,TextureUsage usage = TEXTURE_STATIC);
		virtual bool SetData(unsigned level,int x,int y,int width,int height,const void* data);
		virtual bool SetData(unsigned level,int x,int y,int width,int height,unsigned f1,unsigned f2,unsigned f3,const void* data);
		virtual bool SetData(SharedPtr<YumeImage> image,bool useAlpha = false);


		virtual bool GetData(unsigned level,void* dest) const;
		static unsigned GetDataType(unsigned format);
		static unsigned GetExternalFormat(unsigned format);

		virtual void HandleRenderTargetUpdate();

		virtual bool Create();


		virtual bool IsDataLost();
		virtual void ClearDataLost();

		virtual unsigned GetRowDataSize(int width) const;

		virtual void UpdateParameters();

		virtual void CheckTextureBudget(YumeHash type);

		virtual unsigned GetDataSize(int width,int height) const;

		virtual bool IsCompressed() const;

		
		virtual unsigned GetSRVFormat(unsigned format) { return 0; }
		virtual unsigned GetDSVFormat(unsigned format) { return 0; }
		virtual unsigned GetSRGBFormat(unsigned format) { return 0; }

		const unsigned GetTarget() { return target_; }
	};
}


//----------------------------------------------------------------------------
#endif
