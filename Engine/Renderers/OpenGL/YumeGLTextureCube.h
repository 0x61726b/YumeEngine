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
#ifndef __YumeGLTextureCube_h__
#define __YumeGLTextureCube_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"
#include "Renderer/YumeTextureCube.h"

#include "YumeGLGpuResource.h"
#include "Core/YumeEventHub.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeImage;

	class YumeGLExport YumeGLTextureCube : public YumeTextureCube,public YumeGLResource,public RendererEventListener
	{
	public:
		YumeGLTextureCube();
		virtual ~YumeGLTextureCube();

		virtual void Release();
		virtual bool IsDataLost();
		virtual void ClearDataLost();
		virtual void OnDeviceLost();
		virtual void OnDeviceReset();

		virtual bool SetSize(int size,unsigned format,TextureUsage usage = TEXTURE_STATIC);
		virtual bool SetData(CubeMapFace face,unsigned level,int x,int y,int width,int height,const void* data);
		virtual bool SetData(CubeMapFace face,YumeFile& source);
		virtual bool SetData(CubeMapFace face,SharedPtr<YumeImage> image,bool useAlpha = false);
		virtual bool GetData(CubeMapFace face,unsigned level,void* dest) const;
		virtual bool Create();
		virtual void HandleRenderTargetUpdate();

		virtual bool IsCompressed() const;
		virtual unsigned GetDataSize(int width,int height) const;
		virtual unsigned GetRowDataSize(int width) const;
		virtual void UpdateParameters();
		virtual void CheckTextureBudget(YumeHash type);
		virtual unsigned GetSRVFormat(unsigned format) { return 0; };
		virtual unsigned GetDSVFormat(unsigned format){ return 0; };
		virtual unsigned GetSRGBFormat(unsigned format){ return 0; };

	};
}


//----------------------------------------------------------------------------
#endif
