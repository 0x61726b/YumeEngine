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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeImage_h__
#define __YumeImage_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Math/YumeColor.h"

#include "Math/YumeRect.h"
#include "YumeResource.h"

#include <boost/shared_array.hpp>

struct SDL_Surface;
//----------------------------------------------------------------------------
namespace YumeEngine
{

	static const int COLOR_LUT_SIZE = 16;

	/// Supported compressed image formats.
	enum CompressedFormat
	{
		CF_NONE = 0,
		CF_RGBA,
		CF_DXT1,
		CF_DXT3,
		CF_DXT5,
		CF_ETC1,
		CF_PVRTC_RGB_2BPP,
		CF_PVRTC_RGBA_2BPP,
		CF_PVRTC_RGB_4BPP,
		CF_PVRTC_RGBA_4BPP,
	};

	/// Compressed image mip level.
	struct CompressedLevel
	{
		/// Construct empty.
		CompressedLevel():
			data_(0),
			width_(0),
			height_(0),
			depth_(0),
			blockSize_(0),
			dataSize_(0),
			rowSize_(0),
			rows_(0)
		{
		}

		/// Decompress to RGBA. The destination buffer required is width * height * 4 bytes. Return true if successful.
		bool Decompress(unsigned char* dest);

		/// Compressed image data.
		unsigned char* data_;
		/// Compression format.
		CompressedFormat format_;
		/// Width.
		int width_;
		/// Height.
		int height_;
		/// Depth.
		int depth_;
		/// Block size in bytes.
		unsigned blockSize_;
		/// Total data size in bytes.
		unsigned dataSize_;
		/// Row size in bytes.
		unsigned rowSize_;
		/// Number of rows.
		unsigned rows_;
	};

	class YumeAPIExport YumeImage : public YumeResource
	{
	public:
		/// Construct empty.
		YumeImage();
		/// Destruct.
		virtual ~YumeImage();
		/// Register object factory.

		/// Load resource from stream. May be called from a worker thread. Return true if successful.
		virtual bool BeginLoad(YumeFile& source);
		/// Save the image to a stream. Regardless of original format, the image is saved as png. Compressed image data is not supported. Return true if successful.
		virtual bool Save(YumeFile& dest) const;

		/// Set 2D size and number of color components. Old image data will be destroyed and new data is undefined. Return true if successful.
		bool SetSize(int width,int height,unsigned components);
		/// Set 3D size and number of color components. Old image data will be destroyed and new data is undefined. Return true if successful.
		bool SetSize(int width,int height,int depth,unsigned components);
		/// Set new image data.
		void SetData(const unsigned char* pixelData);
		/// Set a 2D pixel.
		void SetPixel(int x,int y,const YumeColor& color);
		/// Set a 3D pixel.
		void SetPixel(int x,int y,int z,const YumeColor& color);
		/// Set a 2D pixel with an integer color. R component is in the 8 lowest bits.
		void SetPixelInt(int x,int y,unsigned uintColor);
		/// Set a 3D pixel with an integer color. R component is in the 8 lowest bits.
		void SetPixelInt(int x,int y,int z,unsigned uintColor);
		/// Load as color LUT. Return true if successful.
		bool LoadColorLUT(YumeFile& source);
		/// Flip image horizontally. Return true if successful.
		bool FlipHorizontal();
		/// Flip image vertically. Return true if successful.
		bool FlipVertical();
		/// Resize image by bilinear resampling. Return true if successful.
		bool Resize(int width,int height);
		/// Clear the image with a color.
		void Clear(const YumeColor& color);
		/// Clear the image with an integer color. R component is in the 8 lowest bits.
		void ClearInt(unsigned uintColor);
		/// Save in BMP format. Return true if successful.
		bool SaveBMP(const YumeString& fileName) const;
		/// Save in PNG format. Return true if successful.
		bool SavePNG(const YumeString& fileName) const;
		/// Save in TGA format. Return true if successful.
		bool SaveTGA(const YumeString& fileName) const;
		/// Save in JPG format with compression quality. Return true if successful.
		bool SaveJPG(const YumeString& fileName,int quality) const;
		/// Whether this texture is detected as a cubemap, only relevant for DDS.
		bool IsCubemap() const { return cubemap_; }
		/// Whether this texture has been detected as a volume, only relevant for DDS.
		bool IsArray() const { return array_; }
		/// Whether this texture is in sRGB, only relevant for DDS.
		bool IsSRGB() const { return sRGB_; }

		/// Return a 2D pixel color.
		YumeColor GetPixel(int x,int y) const;
		/// Return a 3D pixel color.
		YumeColor GetPixel(int x,int y,int z) const;
		/// Return a 2D pixel integer color. R component is in the 8 lowest bits.
		unsigned GetPixelInt(int x,int y) const;
		/// Return a 3D pixel integer color. R component is in the 8 lowest bits.
		unsigned GetPixelInt(int x,int y,int z) const;
		/// Return a bilinearly sampled 2D pixel color. X and Y have the range 0-1.
		YumeColor GetPixelBilinear(float x,float y) const;
		/// Return a trilinearly sampled 3D pixel color. X, Y and Z have the range 0-1.
		YumeColor GetPixelTrilinear(float x,float y,float z) const;

		/// Return width.
		int GetWidth() const { return width_; }

		/// Return height.
		int GetHeight() const { return height_; }

		/// Return depth.
		int GetDepth() const { return depth_; }

		/// Return number of color components.
		unsigned GetComponents() const { return components_; }

		/// Return pixel data.
		unsigned char* GetData() const { return data_.get(); }

		/// Return whether is compressed.
		bool IsCompressed() const { return compressedFormat_ != CF_NONE; }

		/// Return compressed format.
		CompressedFormat GetCompressedFormat() const { return compressedFormat_; }

		/// Return number of compressed mip levels.
		unsigned GetNumCompressedLevels() const { return numCompressedLevels_; }

		/// Return next mip level by bilinear filtering.
		boost::shared_ptr<YumeImage> GetNextLevel() const;
		/// Return the next sibling image of an array or cubemap.
		boost::shared_ptr<YumeImage> GetNextSibling() const { return nextSibling_; }
		/// Return image converted to 4-component (RGBA) to circumvent modern rendering API's not supporting e.g. the luminance-alpha format.
		boost::shared_ptr<YumeImage> ConvertToRGBA() const;
		/// Return a compressed mip level.
		CompressedLevel GetCompressedLevel(unsigned index) const;
		/// Return subimage from the image by the defined rect or null if failed. 3D images are not supported. You must free the subimage yourself.
		YumeImage* GetSubimage(const IntRect& rect) const;
		/// Return an SDL surface from the image, or null if failed. Only RGB images are supported. Specify rect to only return partial image. You must free the surface yourself.
		SDL_Surface* GetSDLSurface(const IntRect& rect = IntRect::ZERO) const;
		/// Precalculate the mip levels. Used by asynchronous texture loading.
		void PrecalculateLevels();

	private:
		/// Decode an image using stb_image.
		static unsigned char* GetImageData(YumeFile& source,int& width,int& height,unsigned& components);
		/// Free an image file's pixel data.
		static void FreeImageData(unsigned char* pixelData);

		/// Width.
		int width_;
		/// Height.
		int height_;
		/// Depth.
		int depth_;
		/// Number of color components.
		unsigned components_;
		/// Number of compressed mip levels.
		unsigned numCompressedLevels_;
		/// Cubemap status if DDS.
		bool cubemap_;
		/// Texture array status if DDS.
		bool array_;
		/// Data is sRGB.
		bool sRGB_;
		/// Compressed format.
		CompressedFormat compressedFormat_;
		/// Pixel data.
		boost::shared_array<unsigned char> data_;
		/// Precalculated mip level image.
		boost::shared_ptr<YumeImage> nextLevel_;
		/// Next texture array or cube map image.
		boost::shared_ptr<YumeImage> nextSibling_;
	};
}


//----------------------------------------------------------------------------
#endif
