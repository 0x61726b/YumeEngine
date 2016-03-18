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
#ifndef __YumeD3D11TextureCube_h__
#define __YumeD3D11TextureCube_h__
//----------------------------------------------------------------------------
#include "YumeD3D11Required.h"
#include "Renderer/YumeTextureCube.h"

#include "YumeD3D11GpuResource.h"
#include "Core/YumeEventHub.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeImage;

	class YumeD3DExport YumeD3D11TextureCube : public YumeTextureCube,public YumeD3D11Resource,public RendererEventListener
	{
	public:
		YumeD3D11TextureCube();
		virtual ~YumeD3D11TextureCube();

		virtual void Release();

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
		virtual unsigned GetSRVFormat(unsigned format);
		virtual unsigned GetDSVFormat(unsigned format);
		virtual unsigned GetSRGBFormat(unsigned format);

		virtual bool IsDataLost() { return false; };
		virtual void ClearDataLost() { }

	};
}


//----------------------------------------------------------------------------
#endif
