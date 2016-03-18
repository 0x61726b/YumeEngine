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
#ifndef __YumeTextureCube_h__
#define __YumeTextureCube_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeRenderable.h"
#include "YumeTexture.h"
#include "Core/SharedPtr.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeImage;
	class YumeXmlFile;

	class YumeAPIExport YumeTextureCube : public YumeTexture
	{
	public:
		YumeTextureCube();

		virtual ~YumeTextureCube();

		virtual bool BeginLoad(YumeFile& source);
		virtual bool EndLoad();
		virtual void Release() { };

		virtual bool SetSize(int size,unsigned format,TextureUsage usage = TEXTURE_STATIC) = 0;
		virtual bool SetData(CubeMapFace face,unsigned level,int x,int y,int width,int height,const void* data) = 0;
		virtual bool SetData(CubeMapFace face,YumeFile& source) = 0;
		virtual bool SetData(CubeMapFace face,SharedPtr<YumeImage> image,bool useAlpha = false) = 0;
		virtual bool GetData(CubeMapFace face,unsigned level,void* dest) const = 0;
		virtual bool Create() = 0;

		YumeRenderable* GetRenderSurface(CubeMapFace face) const { return renderSurfaces_[face]; }

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;

	protected:
		unsigned faceMemoryUse_[MAX_CUBEMAP_FACES];
		SharedPtr<YumeRenderable> renderSurfaces_[MAX_CUBEMAP_FACES];
		YumeVector<SharedPtr<YumeImage> >::type loadImages_;

		int lockedLevel_;
		CubeMapFace lockedFace_;
		SharedPtr<YumeXmlFile> loadParameters_;
	};
}


//----------------------------------------------------------------------------
#endif
