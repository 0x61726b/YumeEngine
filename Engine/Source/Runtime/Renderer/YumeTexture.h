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
#ifndef __YumeTexture_h__
#define __YumeTexture_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"


static const int MAX_TEXTURE_QUALITY_LEVELS = 3;

#include "Math/YumeColor.h"
#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeTexture : public YumeResource
	{
	public:
		YumeTexture();

		virtual ~YumeTexture();


		/// Set number of requested mip levels. Needs to be called before setting size.
		void SetNumLevels(unsigned levels);
		/// Set filtering mode.
		void SetFilterMode(TextureFilterMode filter);
		/// Set addressing mode by texture coordinate.
		void SetAddressMode(TextureCoordinate coord,TextureAddressMode address);
		/// Set shadow compare mode.
		void SetShadowCompare(bool enable) ;
		/// Set border color for border addressing mode.
		void SetBorderColor(const YumeColor& color);
		/// Set sRGB sampling and writing mode.
		void SetSRGB(bool enable);
		/// Set backup texture to use when rendering to this texture.
		void SetBackupTexture(YumeTexture* texture) ;
		/// Set mip levels to skip on a quality setting when loading. Ensures higher quality levels do not skip more.
		void SetMipsToSkip(int quality,int toSkip) ;

		/// Return texture format.
		unsigned GetFormat() const { return format_; }

		/// Return whether the texture format is compressed.
		virtual bool IsCompressed() const = 0;

		/// Return number of mip levels.
		unsigned GetLevels() const { return levels_; }

		/// Return width.
		int GetWidth() const { return width_; }

		/// Return height.
		int GetHeight() const { return height_; }

		/// Return height.
		int GetDepth() const { return depth_; }

		/// Return filtering mode.
		TextureFilterMode GetFilterMode() const { return filterMode_; }

		/// Return addressing mode by texture coordinate.
		TextureAddressMode GetAddressMode(TextureCoordinate coord) const { return addressMode_[coord]; }

		/// Return whether shadow compare is enabled.
		bool GetShadowCompare() const { return shadowCompare_; }

		/// Return border color.
		const YumeColor& GetBorderColor() const { return borderColor_; }

		/// Return whether is using sRGB sampling and writing.
		bool GetSRGB() const { return sRGB_; }

		/// Return backup texture.
		YumeTexture* GetBackupTexture() const { return backupTexture_.get(); }

		/// Return mip levels to skip on a quality setting when loading.
		int GetMipsToSkip(int quality) const;
		/// Return mip level width, or 0 if level does not exist.
		int GetLevelWidth(unsigned level) const;
		/// Return mip level width, or 0 if level does not exist.
		int GetLevelHeight(unsigned level) const;
		/// Return mip level depth, or 0 if level does not exist.
		int GetLevelDepth(unsigned level) const;

		/// Return texture usage type.
		TextureUsage GetUsage() const { return usage_; }

		/// Return data size in bytes for a rectangular region.
		virtual unsigned GetDataSize(int width,int height) const = 0;
		/// Return data size in bytes for a volume region.
		unsigned GetDataSize(int width,int height,int depth) const;
		/// Return data size in bytes for a pixel or block row.
		virtual unsigned GetRowDataSize(int width) const = 0;
		/// Return number of image components required to receive pixel data from GetData(), or 0 for compressed images.
		unsigned GetComponents() const;

		/// Return whether the parameters are dirty.
		bool GetParametersDirty() const { return parametersDirty_ || !sampler_; }

		/// Set additional parameters from an XML file.
		void SetParameters();
		/// Mark parameters dirty. Called by Graphics.
		void SetParametersDirty();
		/// Create sampler state object after parameters have been changed. Called by Graphics when assigning the texture.
		virtual void UpdateParameters() = 0;

		/// Return shader resource view.
		void* GetShaderResourceView() const { return shaderResourceView_; }

		/// Return sampler state object.
		void* GetSampler() const { return sampler_; }


		/// Check whether texture memory budget has been exceeded. Free unused materials in that case to release the texture references.
		virtual void CheckTextureBudget(YumeHash type) = 0;

		/// Check maximum allowed mip levels for a specific texture size.
		static unsigned CheckMaxLevels(int width,int height,unsigned requestedLevels);
		/// Check maximum allowed mip levels for a specific 3D texture size.
		static unsigned CheckMaxLevels(int width,int height,int depth,unsigned requestedLevels);
		/// Return the shader resource view format corresponding to a texture format. Handles conversion of typeless depth texture formats.
		virtual unsigned GetSRVFormat(unsigned format) = 0;
		/// Return the depth-stencil view format corresponding to a texture format. Handles conversion of typeless depth texture formats.
		virtual unsigned GetDSVFormat(unsigned format) = 0;
		/// Convert format to sRGB.
		virtual unsigned GetSRGBFormat(unsigned format) = 0;
	protected:
		/// Shader resource view.
		void* shaderResourceView_;
		//OpenGL target
		unsigned target_;
		/// Sampler state object.
		void* sampler_;
		/// Texture format.
		unsigned format_;
		/// Texture usage type.
		TextureUsage usage_;
		/// Current mip levels.
		unsigned levels_;
		/// Requested mip levels.
		unsigned requestedLevels_;
		/// Texture width.
		int width_;
		/// Texture height.
		int height_;
		/// Texture depth.
		int depth_;
		/// Shadow compare mode.
		bool shadowCompare_;
		/// Filtering mode.
		TextureFilterMode filterMode_;
		/// Addressing mode.
		TextureAddressMode addressMode_[MAX_COORDS];
		/// Mip levels to skip when loading per texture quality setting.
		unsigned mipsToSkip_[MAX_TEXTURE_QUALITY_LEVELS];
		/// Border color.
		YumeColor borderColor_;
		/// sRGB sampling and writing mode flag.
		bool sRGB_;
		/// Parameters dirty flag.
		bool parametersDirty_;
		/// Backup texture.
		SharedPtr<YumeTexture> backupTexture_;

	};
}


//----------------------------------------------------------------------------
#endif
