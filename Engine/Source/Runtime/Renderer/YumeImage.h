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
#include "Core/SharedPtr.h"

struct SDL_Surface;
//----------------------------------------------------------------------------
namespace YumeEngine
{
	static const int COLOR_LUT_SIZE = 16;

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
	struct YumeAPIExport CompressedLevel
	{
		
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
	
		bool Decompress(unsigned char* dest);
		unsigned char* data_;
		CompressedFormat format_;
		int width_;
		int height_;
		int depth_;
		unsigned blockSize_;
		unsigned dataSize_;
		unsigned rowSize_;
		unsigned rows_;
	};

	class YumeAPIExport YumeImage : public YumeResource
	{
	public:
		
		YumeImage();
		
		virtual ~YumeImage();
		

		
		virtual bool BeginLoad(YumeFile& source);

		virtual bool EndLoad();
		
		virtual bool Save(YumeFile& dest) const;

		
		bool SetSize(int width,int height,unsigned components);
		
		bool SetSize(int width,int height,int depth,unsigned components);
		
		void SetData(const unsigned char* pixelData);
		
		void SetPixel(int x,int y,const YumeColor& color);
		
		void SetPixel(int x,int y,int z,const YumeColor& color);
		
		void SetPixelInt(int x,int y,unsigned uintColor);
		
		void SetPixelInt(int x,int y,int z,unsigned uintColor);
		
		bool LoadColorLUT(YumeFile& source);
		
		bool FlipHorizontal();
		
		bool FlipVertical();
		
		bool Resize(int width,int height);
		
		void Clear(const YumeColor& color);
		
		void ClearInt(unsigned uintColor);
		
		bool SaveBMP(const YumeString& fileName) const;
		
		bool SavePNG(const YumeString& fileName) const;
		
		bool SaveTGA(const YumeString& fileName) const;
		
		bool SaveJPG(const YumeString& fileName,int quality) const;
		
		bool IsCubemap() const { return cubemap_; }
		
		bool IsArray() const { return array_; }
		
		bool IsSRGB() const { return sRGB_; }

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;
		
		YumeColor GetPixel(int x,int y) const;
		
		YumeColor GetPixel(int x,int y,int z) const;
		
		unsigned GetPixelInt(int x,int y) const;
		
		unsigned GetPixelInt(int x,int y,int z) const;
		
		YumeColor GetPixelBilinear(float x,float y) const;
		
		YumeColor GetPixelTrilinear(float x,float y,float z) const;

		
		int GetWidth() const { return width_; }

		
		int GetHeight() const { return height_; }

		
		int GetDepth() const { return depth_; }

		
		unsigned GetComponents() const { return components_; }

		
		unsigned char* GetData() const { return data_.get(); }

		
		bool IsCompressed() const { return compressedFormat_ != CF_NONE; }

		
		CompressedFormat GetCompressedFormat() const { return compressedFormat_; }

		
		unsigned GetNumCompressedLevels() const { return numCompressedLevels_; }

		
		SharedPtr<YumeImage> GetNextLevel() const;
		
		SharedPtr<YumeImage> GetNextSibling() const { return nextSibling_; }
		
		SharedPtr<YumeImage> ConvertToRGBA() const;
		
		CompressedLevel GetCompressedLevel(unsigned index) const;
		
		YumeImage* GetSubimage(const IntRect& rect) const;
		
		SDL_Surface* GetSDLSurface(const IntRect& rect = IntRect::ZERO) const;
		
		void PrecalculateLevels();

	private:
		
		static unsigned char* GetImageData(YumeFile& source,int& width,int& height,unsigned& components);
		
		static void FreeImageData(unsigned char* pixelData);

		
		int width_;
		
		int height_;
		
		int depth_;
		
		unsigned components_;
		
		unsigned numCompressedLevels_;
		
		bool cubemap_;
		
		bool array_;
		
		bool sRGB_;
		
		CompressedFormat compressedFormat_;
		
		boost::shared_array<unsigned char> data_;
		
		SharedPtr<YumeImage> nextLevel_;
		
		SharedPtr<YumeImage> nextSibling_;
	};
}


//----------------------------------------------------------------------------
#endif
