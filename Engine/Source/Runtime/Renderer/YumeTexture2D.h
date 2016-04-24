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
#ifndef __YumeTexture2D_h__
#define __YumeTexture2D_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeTexture.h"
#include "Core/YumeVariant.h"
#include "Renderer/YumeRenderable.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeImage;
	class YumeXmlFile;

	class YumeAPIExport YumeTexture2D : public YumeTexture
	{
	public:
		YumeTexture2D();

		virtual ~YumeTexture2D();

		virtual bool BeginLoad(YumeFile& source);
		virtual bool EndLoad();
		virtual void Release() { };

		virtual bool SetSize(int width,int height,unsigned format,TextureUsage usage = TEXTURE_STATIC,int arraySize = 1,int mips = 1) = 0;
		virtual bool SetData(unsigned level,int x,int y,int width,int height,const void* data) = 0;
		virtual bool SetData(unsigned level,int x,int y,int width,int height,unsigned f1,unsigned f2,unsigned f3,const void* data) = 0;
		virtual bool SetData(SharedPtr<YumeImage> image,bool useAlpha = false) = 0;
		virtual bool GetData(unsigned level,void* dest) const = 0;

		
		SharedPtr<YumeRenderable> GetRenderSurface() const { return renderSurface_; }

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;

		
		virtual bool Create() = 0;
	protected:
		
		SharedPtr<YumeRenderable> renderSurface_;
		SharedPtr<YumeImage> loadImage_;
		SharedPtr<YumeXmlFile> loadParameters_;
	};

	typedef YumeTexture2D* Texture2DPtr;
}


//----------------------------------------------------------------------------
#endif
