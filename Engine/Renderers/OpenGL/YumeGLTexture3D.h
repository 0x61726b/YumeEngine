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
#ifndef __YumeGLTexture3D_h__
#define __YumeGLTexture3D_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"
#include "Renderer/YumeTexture3D.h"
#include "Core/YumeEventHub.h"
#include "YumeGLGpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGLExport YumeGLTexture3D : public YumeTexture3D,public YumeGLResource
	{
	public:
		YumeGLTexture3D();
		virtual ~YumeGLTexture3D();

		virtual void Release();

		virtual bool SetSize(int width,int height,int depth,unsigned format,TextureUsage usage = TEXTURE_STATIC);
		virtual bool SetData(unsigned level,int x,int y,int z,int width,int height,int depth,const void* data) ;
		virtual bool SetData(SharedPtr<YumeImage> image,bool useAlpha = false) ;
		virtual bool GetData(unsigned level,void* dest) const;

		virtual bool Create();

		virtual unsigned GetDataSize(int width,int height) const;
		virtual unsigned GetDataSize(int width,int height,int depth) const;
		virtual unsigned GetRowDataSize(int width) const;



		virtual void UpdateParameters();

		virtual void CheckTextureBudget(YumeHash type);
		virtual unsigned GetSRVFormat(unsigned format);
		virtual unsigned GetDSVFormat(unsigned format);
		virtual unsigned GetSRGBFormat(unsigned format);
		virtual bool IsCompressed() const;

		virtual bool IsDataLost();
		virtual void ClearDataLost();
		virtual void OnDeviceLost();
		virtual void OnDeviceReset();

	};
}


//----------------------------------------------------------------------------
#endif
