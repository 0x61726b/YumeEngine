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

	class YumeAPIExport YumeTexture2D : public YumeTexture
	{
	public:
		YumeTexture2D();

		virtual ~YumeTexture2D();

		/// Load resource from stream. May be called from a worker thread. Return true if successful.
		virtual bool BeginLoad(YumeFile& source);
		/// Finish resource loading. Always called from the main thread. Return true if successful.
		virtual bool EndLoad();

		virtual void Release() { };

		/// Set size, format and usage. Zero size will follow application window size. Return true if successful.
		virtual bool SetSize(int width,int height,unsigned format,TextureUsage usage = TEXTURE_STATIC) = 0;
		/// Set data either partially or fully on a mip level. Return true if successful.
		virtual bool SetData(unsigned level,int x,int y,int width,int height,const void* data) = 0;
		/// Set data from an image. Return true if successful. Optionally make a single channel image alpha-only.
		virtual bool SetData(SharedPtr<YumeImage> image,bool useAlpha = false) = 0;

		virtual bool GetData(unsigned level,void* dest) const = 0;

		/// Return render surface.
		YumeRenderable* GetRenderSurface() const { return renderSurface_.get(); }

		static YumeHash GetType();
		static YumeHash texture2Dhash_;

		/// Create texture.
		virtual bool Create() = 0;
		/// Handle render surface update event.
		virtual void HandleRenderSurfaceUpdate(YumeHash eventType,VariantMap& eventData) = 0;

	protected:
		/// Render surface.
		SharedPtr<YumeRenderable> renderSurface_;
		/// Image file acquired during BeginLoad.
		SharedPtr<YumeImage> loadImage_;
	};
}


//----------------------------------------------------------------------------
#endif
