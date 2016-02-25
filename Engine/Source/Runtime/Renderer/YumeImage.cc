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
#include "YumeHeaders.h"
#include "YumeImage.h"

#include "Core/YumeFile.h"

#include "Core/YumeDecompresser.h"

#include <Logging/logging.h>

#include <JO/jo_jpeg.h>
#include <SDL_surface.h>
#include <STB/stb_image.h>
#include <STB/stb_image_write.h>


extern "C" unsigned char* stbi_write_png_to_mem(unsigned char* pixels,int stride_bytes,int x,int y,int n,int* out_len);

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((unsigned)(ch0) | ((unsigned)(ch1) << 8) | ((unsigned)(ch2) << 16) | ((unsigned)(ch3) << 24))
#endif

#define FOURCC_DXT1 (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2 (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3 (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4 (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5 (MAKEFOURCC('D','X','T','5'))
#define FOURCC_DX10 (MAKEFOURCC('D','X','1','0'))

static const unsigned DDSCAPS_COMPLEX = 0x00000008U;
static const unsigned DDSCAPS_TEXTURE = 0x00001000U;
static const unsigned DDSCAPS_MIPMAP = 0x00400000U;
static const unsigned DDSCAPS2_VOLUME = 0x00200000U;
static const unsigned DDSCAPS2_CUBEMAP = 0x00000200U;

static const unsigned DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400U;
static const unsigned DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800U;
static const unsigned DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000U;
static const unsigned DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000U;
static const unsigned DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000U;
static const unsigned DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000U;
static const unsigned DDSCAPS2_CUBEMAP_ALL_FACES = 0x0000FC00U;

// DX10 flags
static const unsigned DDS_DIMENSION_TEXTURE1D = 2;
static const unsigned DDS_DIMENSION_TEXTURE2D = 3;
static const unsigned DDS_DIMENSION_TEXTURE3D = 4;

static const unsigned DDS_RESOURCE_MISC_TEXTURECUBE = 0x4;

static const unsigned DDS_DXGI_FORMAT_R8G8B8A8_UNORM = 28;
static const unsigned DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 26;
static const unsigned DDS_DXGI_FORMAT_BC1_UNORM = 71;
static const unsigned DDS_DXGI_FORMAT_BC1_UNORM_SRGB = 72;
static const unsigned DDS_DXGI_FORMAT_BC2_UNORM = 74;
static const unsigned DDS_DXGI_FORMAT_BC2_UNORM_SRGB = 75;
static const unsigned DDS_DXGI_FORMAT_BC3_UNORM = 77;
static const unsigned DDS_DXGI_FORMAT_BC3_UNORM_SRGB = 78;

namespace YumeEngine
{
	/// DirectDraw color key definition.
	struct DDColorKey
	{
		unsigned dwColorSpaceLowValue_;
		unsigned dwColorSpaceHighValue_;
	};

	/// DirectDraw pixel format definition.
	struct DDPixelFormat
	{
		unsigned dwSize_;
		unsigned dwFlags_;
		unsigned dwFourCC_;
		union
		{
			unsigned dwRGBBitCount_;
			unsigned dwYUVBitCount_;
			unsigned dwZBufferBitDepth_;
			unsigned dwAlphaBitDepth_;
			unsigned dwLuminanceBitCount_;
			unsigned dwBumpBitCount_;
			unsigned dwPrivateFormatBitCount_;
		};
		union
		{
			unsigned dwRBitMask_;
			unsigned dwYBitMask_;
			unsigned dwStencilBitDepth_;
			unsigned dwLuminanceBitMask_;
			unsigned dwBumpDuBitMask_;
			unsigned dwOperations_;
		};
		union
		{
			unsigned dwGBitMask_;
			unsigned dwUBitMask_;
			unsigned dwZBitMask_;
			unsigned dwBumpDvBitMask_;
			struct
			{
				unsigned short wFlipMSTypes_;
				unsigned short wBltMSTypes_;
			} multiSampleCaps_;
		};
		union
		{
			unsigned dwBBitMask_;
			unsigned dwVBitMask_;
			unsigned dwStencilBitMask_;
			unsigned dwBumpLuminanceBitMask_;
		};
		union
		{
			unsigned dwRGBAlphaBitMask_;
			unsigned dwYUVAlphaBitMask_;
			unsigned dwLuminanceAlphaBitMask_;
			unsigned dwRGBZBitMask_;
			unsigned dwYUVZBitMask_;
		};
	};

	/// DirectDraw surface capabilities.
	struct DDSCaps2
	{
		unsigned dwCaps_;
		unsigned dwCaps2_;
		unsigned dwCaps3_;
		union
		{
			unsigned dwCaps4_;
			unsigned dwVolumeDepth_;
		};
	};

	struct DDSHeader10
	{
		unsigned dxgiFormat;
		unsigned resourceDimension;
		unsigned miscFlag;
		unsigned arraySize;
		unsigned reserved;
	};

	/// DirectDraw surface description.
	struct DDSurfaceDesc2
	{
		unsigned dwSize_;
		unsigned dwFlags_;
		unsigned dwHeight_;
		unsigned dwWidth_;
		union
		{
			unsigned lPitch_;
			unsigned dwLinearSize_;
		};
		union
		{
			unsigned dwBackBufferCount_;
			unsigned dwDepth_;
		};
		union
		{
			unsigned dwMipMapCount_;
			unsigned dwRefreshRate_;
			unsigned dwSrcVBHandle_;
		};
		unsigned dwAlphaBitDepth_;
		unsigned dwReserved_;
		unsigned lpSurface_; // Do not define as a void pointer, as it is 8 bytes in a 64bit build
		union
		{
			DDColorKey ddckCKDestOverlay_;
			unsigned dwEmptyFaceColor_;
		};
		DDColorKey ddckCKDestBlt_;
		DDColorKey ddckCKSrcOverlay_;
		DDColorKey ddckCKSrcBlt_;
		union
		{
			DDPixelFormat ddpfPixelFormat_;
			unsigned dwFVF_;
		};
		DDSCaps2 ddsCaps_;
		unsigned dwTextureStage_;
	};

	bool CompressedLevel::Decompress(unsigned char* dest)
	{
		if(!data_)
			return false;

		switch(format_)
		{
		case CF_DXT1:
		case CF_DXT3:
		case CF_DXT5:
			DecompressImageDXT(dest,data_,width_,height_,depth_,format_);
			return true;

		case CF_ETC1:
			DecompressImageETC(dest,data_,width_,height_);
			return true;

		case CF_PVRTC_RGB_2BPP:
		case CF_PVRTC_RGBA_2BPP:
		case CF_PVRTC_RGB_4BPP:
		case CF_PVRTC_RGBA_4BPP:
			DecompressImagePVRTC(dest,data_,width_,height_,format_);
			return true;

		default:
			// Unknown format
			return false;
		}
	}


	YumeImage::YumeImage():
		width_(0),
		height_(0),
		depth_(0),
		components_(0),
		cubemap_(false),
		array_(false),
		sRGB_(false)
	{
	}

	YumeImage::~YumeImage()
	{
	}

	bool YumeImage::BeginLoad(YumeFile& source)
	{
		YumeString fileExtension = source.GetFileExtension();

		if(fileExtension == "DDS ")
		{
			// DDS compressed format
			DDSurfaceDesc2 ddsd;
			source.Read(&ddsd,sizeof(ddsd));

			// DDS DX10+
			const bool hasDXGI = ddsd.ddpfPixelFormat_.dwFourCC_ == FOURCC_DX10;
			DDSHeader10 dxgiHeader;
			if(hasDXGI)
				source.Read(&dxgiHeader,sizeof(dxgiHeader));

			unsigned fourCC = ddsd.ddpfPixelFormat_.dwFourCC_;

			// If the DXGI header is available then remap formats and check sRGB
			if(hasDXGI)
			{
				switch(dxgiHeader.dxgiFormat)
				{
				case DDS_DXGI_FORMAT_BC1_UNORM:
				case DDS_DXGI_FORMAT_BC1_UNORM_SRGB:
					fourCC = FOURCC_DXT1;
					break;
				case DDS_DXGI_FORMAT_BC2_UNORM:
				case DDS_DXGI_FORMAT_BC2_UNORM_SRGB:
					fourCC = FOURCC_DXT3;
					break;
				case DDS_DXGI_FORMAT_BC3_UNORM:
				case DDS_DXGI_FORMAT_BC3_UNORM_SRGB:
					fourCC = FOURCC_DXT5;
					break;
				case DDS_DXGI_FORMAT_R8G8B8A8_UNORM:
				case DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
					fourCC = 0;
					break;
				default:
					YUMELOG_ERROR("Unrecognized DDS DXGI image format");
					return false;
				}

				// Check the internal sRGB formats
				if(dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_BC1_UNORM_SRGB ||
					dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_BC2_UNORM_SRGB ||
					dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_BC3_UNORM_SRGB ||
					dxgiHeader.dxgiFormat == DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
				{
					sRGB_ = true;
				}
			}
			switch(fourCC)
			{
			case FOURCC_DXT1:
				compressedFormat_ = CF_DXT1;
				components_ = 3;
				break;

			case FOURCC_DXT3:
				compressedFormat_ = CF_DXT3;
				components_ = 4;
				break;

			case FOURCC_DXT5:
				compressedFormat_ = CF_DXT5;
				components_ = 4;
				break;

			case 0:
				if(ddsd.ddpfPixelFormat_.dwRGBBitCount_ != 32 && ddsd.ddpfPixelFormat_.dwRGBBitCount_ != 24 &&
					ddsd.ddpfPixelFormat_.dwRGBBitCount_ != 16)
				{
					YUMELOG_ERROR("Unsupported DDS pixel byte size");
					return false;
				}
				compressedFormat_ = CF_RGBA;
				components_ = 4;
				break;

			default:
				YUMELOG_ERROR("Unrecognized DDS image format");
				return false;
			}

			// Is it a cube map or texture array? If so determine the size of the image chain.
			cubemap_ = (ddsd.ddsCaps_.dwCaps2_ & DDSCAPS2_CUBEMAP_ALL_FACES) != 0 || (hasDXGI && (dxgiHeader.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE) != 0);
			unsigned imageChainCount = 1;
			if(cubemap_)
				imageChainCount = 6;
			else if(hasDXGI && dxgiHeader.arraySize > 1)
			{
				imageChainCount = dxgiHeader.arraySize;
				array_ = true;
			}

			// Calculate the size of the data
			unsigned dataSize = 0;
			if(compressedFormat_ != CF_RGBA)
			{
				const unsigned blockSize = compressedFormat_ == CF_DXT1 ? 8 : 16; //DXT1/BC1 is 8 bytes, DXT3/BC2 and DXT5/BC3 are 16 bytes
				// Add 3 to ensure valid block: ie 2x2 fits uses a whole 4x4 block
				unsigned blocksWide = (ddsd.dwWidth_ + 3) / 4;
				unsigned blocksHeight = (ddsd.dwHeight_ + 3) / 4;
				dataSize = blocksWide * blocksHeight * blockSize;

				// Calculate mip data size
				unsigned x = ddsd.dwWidth_ / 2;
				unsigned y = ddsd.dwHeight_ / 2;
				unsigned z = ddsd.dwDepth_ / 2;
				for(unsigned level = ddsd.dwMipMapCount_; level > 1; x /= 2,y /= 2,z /= 2,--level)
				{
					blocksWide = (std::max((int)x,1) + 3) / 4;
					blocksHeight = (std::max((int)y,1) + 3) / 4;
					dataSize += blockSize * blocksWide * blocksHeight * std::max((int)z,1);
				}
			}
			else
			{
				dataSize = (ddsd.ddpfPixelFormat_.dwRGBBitCount_ / 8) * ddsd.dwWidth_ * ddsd.dwHeight_ * std::max((int)ddsd.dwDepth_,1);
				// Calculate mip data size
				unsigned x = ddsd.dwWidth_ / 2;
				unsigned y = ddsd.dwHeight_ / 2;
				unsigned z = ddsd.dwDepth_ / 2;
				for(unsigned level = ddsd.dwMipMapCount_; level > 1; x /= 2,y /= 2,z /= 2,--level)
					dataSize += (ddsd.ddpfPixelFormat_.dwRGBBitCount_ / 8) * std::max((int)x,1) * std::max((int)y,1) * std::max((int)z,1);
			}

			// Do not use a shared ptr here, in case nothing is refcounting the image outside this function.
			// A raw pointer is fine as the image chain (if needed) uses shared ptr's properly
			YumeImage* currentImage = this;

			for(unsigned faceIndex = 0; faceIndex < imageChainCount; ++faceIndex)
			{
				currentImage->data_ = boost::shared_array<unsigned char>(new unsigned char[dataSize]);
				currentImage->cubemap_ = cubemap_;
				currentImage->array_ = array_;
				currentImage->components_ = components_;
				currentImage->compressedFormat_ = compressedFormat_;
				currentImage->width_ = ddsd.dwWidth_;
				currentImage->height_ = ddsd.dwHeight_;
				currentImage->depth_ = ddsd.dwDepth_;

				currentImage->numCompressedLevels_ = ddsd.dwMipMapCount_;
				if(!currentImage->numCompressedLevels_)
					currentImage->numCompressedLevels_ = 1;

				// Memory use needs to be exact per image as it's used for verifying the data size in GetCompressedLevel()
				// even though it would be more proper for the first image to report the size of all siblings combined
				currentImage->SetMemoryUsage(dataSize);

				source.Read(currentImage->data_.get(),dataSize);

				if(faceIndex < imageChainCount - 1)
				{
					// Build the image chain
					boost::shared_ptr<YumeImage> nextImage(new YumeImage());
					currentImage->nextSibling_ = nextImage;
					currentImage = nextImage.get();
				}
			}

			// If uncompressed DDS, convert the data to 8bit RGBA as the texture classes can not currently use eg. RGB565 format
			if(compressedFormat_ == CF_RGBA)
			{
				currentImage = this;

				while(currentImage)
				{
					unsigned sourcePixelByteSize = ddsd.ddpfPixelFormat_.dwRGBBitCount_ >> 3;
					unsigned numPixels = dataSize / sourcePixelByteSize;

#define ADJUSTSHIFT(mask, l, r) \
                if (mask && mask >= 0x100) \
				                { \
								                    while ((mask >> r) >= 0x100) \
                    ++r; \
				                } \
			                else if (mask && mask < 0x80) \
					                { \
									                    while ((mask << l) < 0x80) \
                    ++l; \
					                }

					unsigned rShiftL = 0,gShiftL = 0,bShiftL = 0,aShiftL = 0;
					unsigned rShiftR = 0,gShiftR = 0,bShiftR = 0,aShiftR = 0;
					unsigned rMask = ddsd.ddpfPixelFormat_.dwRBitMask_;
					unsigned gMask = ddsd.ddpfPixelFormat_.dwGBitMask_;
					unsigned bMask = ddsd.ddpfPixelFormat_.dwBBitMask_;
					unsigned aMask = ddsd.ddpfPixelFormat_.dwRGBAlphaBitMask_;
					ADJUSTSHIFT(rMask,rShiftL,rShiftR)
						ADJUSTSHIFT(gMask,gShiftL,gShiftR)
						ADJUSTSHIFT(bMask,bShiftL,bShiftR)
						ADJUSTSHIFT(aMask,aShiftL,aShiftR)

						boost::shared_array<unsigned char> rgbaData(new unsigned char[numPixels * 4]);

					switch(sourcePixelByteSize)
					{
					case 4:
					{
						unsigned* src = (unsigned*)currentImage->data_.get();
						unsigned char* dest = rgbaData.get();

						while(numPixels--)
						{
							unsigned pixels = *src++;
							*dest++ = ((pixels & rMask) << rShiftL) >> rShiftR;
							*dest++ = ((pixels & gMask) << gShiftL) >> gShiftR;
							*dest++ = ((pixels & bMask) << bShiftL) >> bShiftR;
							*dest++ = ((pixels & aMask) << aShiftL) >> aShiftR;
						}
					}
					break;

					case 3:
					{
						unsigned char* src = currentImage->data_.get();
						unsigned char* dest = rgbaData.get();

						while(numPixels--)
						{
							unsigned pixels = src[0] | (src[1] << 8) | (src[2] << 16);
							src += 3;
							*dest++ = ((pixels & rMask) << rShiftL) >> rShiftR;
							*dest++ = ((pixels & gMask) << gShiftL) >> gShiftR;
							*dest++ = ((pixels & bMask) << bShiftL) >> bShiftR;
							*dest++ = ((pixels & aMask) << aShiftL) >> aShiftR;
						}
					}
					break;

					default:
					{
						unsigned short* src = (unsigned short*)currentImage->data_.get();
						unsigned char* dest = rgbaData.get();

						while(numPixels--)
						{
							unsigned short pixels = *src++;
							*dest++ = ((pixels & rMask) << rShiftL) >> rShiftR;
							*dest++ = ((pixels & gMask) << gShiftL) >> gShiftR;
							*dest++ = ((pixels & bMask) << bShiftL) >> bShiftR;
							*dest++ = ((pixels & aMask) << aShiftL) >> aShiftR;
						}
					}
					break;
					}

					// Replace with converted data
					currentImage->data_ = rgbaData;
					currentImage->SetMemoryUsage(numPixels * 4);
					currentImage = currentImage->GetNextSibling().get();
				}
			}
		}
		else if(fileExtension == "\253KTX")
		{
			// ToDo(arkenthera): implement KTX
			//source.Seek(12);

			//unsigned endianness = source.ReadUInt();
			//unsigned type = source.ReadUInt();
			///* unsigned typeSize = */ source.ReadUInt();
			//unsigned format = source.ReadUInt();
			//unsigned internalFormat = source.ReadUInt();
			///* unsigned baseInternalFormat = */ source.ReadUInt();
			//unsigned width = source.ReadUInt();
			//unsigned height = source.ReadUInt();
			//unsigned depth = source.ReadUInt();
			///* unsigned arrayElements = */ source.ReadUInt();
			//unsigned faces = source.ReadUInt();
			//unsigned mipmaps = source.ReadUInt();
			//unsigned keyValueBytes = source.ReadUInt();

			//if(endianness != 0x04030201)
			//{
			//	URHO3D_LOGERROR("Big-endian KTX files not supported");
			//	return false;
			//}

			//if(type != 0 || format != 0)
			//{
			//	URHO3D_LOGERROR("Uncompressed KTX files not supported");
			//	return false;
			//}

			//if(faces > 1 || depth > 1)
			//{
			//	URHO3D_LOGERROR("3D or cube KTX files not supported");
			//	return false;
			//}

			//if(mipmaps == 0)
			//{
			//	URHO3D_LOGERROR("KTX files without explicitly specified mipmap count not supported");
			//	return false;
			//}

			//switch(internalFormat)
			//{
			//case 0x83f1:
			//	compressedFormat_ = CF_DXT1;
			//	components_ = 4;
			//	break;

			//case 0x83f2:
			//	compressedFormat_ = CF_DXT3;
			//	components_ = 4;
			//	break;

			//case 0x83f3:
			//	compressedFormat_ = CF_DXT5;
			//	components_ = 4;
			//	break;

			//case 0x8d64:
			//	compressedFormat_ = CF_ETC1;
			//	components_ = 3;
			//	break;

			//case 0x8c00:
			//	compressedFormat_ = CF_PVRTC_RGB_4BPP;
			//	components_ = 3;
			//	break;

			//case 0x8c01:
			//	compressedFormat_ = CF_PVRTC_RGB_2BPP;
			//	components_ = 3;
			//	break;

			//case 0x8c02:
			//	compressedFormat_ = CF_PVRTC_RGBA_4BPP;
			//	components_ = 4;
			//	break;

			//case 0x8c03:
			//	compressedFormat_ = CF_PVRTC_RGBA_2BPP;
			//	components_ = 4;
			//	break;

			//default:
			//	compressedFormat_ = CF_NONE;
			//	break;
			//}

			//if(compressedFormat_ == CF_NONE)
			//{
			//	YUMELOG_ERROR("Unsupported texture format in KTX file");
			//	return false;
			//}

			//source.Seek(source.GetPosition() + keyValueBytes);
			//unsigned dataSize = (unsigned)(source.GetSize() - source.GetPosition() - mipmaps * sizeof(unsigned));

			//data_ = new unsigned char[dataSize];
			//width_ = width;
			//height_ = height;
			//numCompressedLevels_ = mipmaps;

			//unsigned dataOffset = 0;
			//for(unsigned i = 0; i < mipmaps; ++i)
			//{
			//	unsigned levelSize = source.ReadUInt();
			//	if(levelSize + dataOffset > dataSize)
			//	{
			//		URHO3D_LOGERROR("KTX mipmap level data size exceeds file size");
			//		return false;
			//	}

			//	source.Read(&data_[dataOffset],levelSize);
			//	dataOffset += levelSize;
			//	if(source.GetPosition() & 3)
			//		source.Seek((source.GetPosition() + 3) & 0xfffffffc);
			//}

			//SetMemoryUsage(dataSize);
		}
		else if(fileExtension == "PVR\3")
		{
			///* unsigned flags = */ source.ReadUInt();
			//unsigned pixelFormatLo = source.ReadUInt();
			///* unsigned pixelFormatHi = */ source.ReadUInt();
			///* unsigned colourSpace = */ source.ReadUInt();
			///* unsigned channelType = */ source.ReadUInt();
			//unsigned height = source.ReadUInt();
			//unsigned width = source.ReadUInt();
			//unsigned depth = source.ReadUInt();
			///* unsigned numSurfaces = */ source.ReadUInt();
			//unsigned numFaces = source.ReadUInt();
			//unsigned mipmapCount = source.ReadUInt();
			//unsigned metaDataSize = source.ReadUInt();

			//if(depth > 1 || numFaces > 1)
			//{
			//	YUMELOG_ERROR("3D or cube PVR files not supported");
			//	return false;
			//}

			//if(mipmapCount == 0)
			//{
			//	YUMELOG_ERROR("PVR files without explicitly specified mipmap count not supported");
			//	return false;
			//}

			//switch(pixelFormatLo)
			//{
			//case 0:
			//	compressedFormat_ = CF_PVRTC_RGB_2BPP;
			//	components_ = 3;
			//	break;

			//case 1:
			//	compressedFormat_ = CF_PVRTC_RGBA_2BPP;
			//	components_ = 4;
			//	break;

			//case 2:
			//	compressedFormat_ = CF_PVRTC_RGB_4BPP;
			//	components_ = 3;
			//	break;

			//case 3:
			//	compressedFormat_ = CF_PVRTC_RGBA_4BPP;
			//	components_ = 4;
			//	break;

			//case 6:
			//	compressedFormat_ = CF_ETC1;
			//	components_ = 3;
			//	break;

			//case 7:
			//	compressedFormat_ = CF_DXT1;
			//	components_ = 4;
			//	break;

			//case 9:
			//	compressedFormat_ = CF_DXT3;
			//	components_ = 4;
			//	break;

			//case 11:
			//	compressedFormat_ = CF_DXT5;
			//	components_ = 4;
			//	break;

			//default:
			//	compressedFormat_ = CF_NONE;
			//	break;
			//}

			//if(compressedFormat_ == CF_NONE)
			//{
			//	YUMELOG_ERROR("Unsupported texture format in PVR file");
			//	return false;
			//}

			// ToDo(arkenthera): implement PVR
			//source.Seek(source.GetPosition() + metaDataSize);
			/*unsigned dataSize = source.GetSize() - source.GetPosition();

			data_ = boost::shared_array<unsigned char>(new unsigned char[dataSize]);
			width_ = width;
			height_ = height;
			numCompressedLevels_ = mipmapCount;

			source.Read(data_.get(),dataSize);
			SetMemoryUsage(dataSize);*/
		}
		else
		{
			// Not DDS, KTX or PVR, use STBImage to load other image formats as uncompressed
			source.Seek(0);
			int width,height;
			unsigned components;
			unsigned char* pixelData = GetImageData(source,width,height,components);
			if(!pixelData)
			{
				YUMELOG_ERROR("Could not load image " + source.GetName() + ": " + YumeString(stbi_failure_reason()));
				return false;
			}
			SetSize(width,height,components);
			SetData(pixelData);
			FreeImageData(pixelData);
		}
		return true;
	}

	bool YumeImage::Save(YumeFile& source) const
	{
		int len;
		unsigned char* png = stbi_write_png_to_mem(data_.get(),0,width_,height_,components_,&len);
		bool success = source.Write(png,(unsigned)len) == (unsigned)len;
		free(png);
		return success;
	}


	bool YumeImage::SetSize(int width,int height,unsigned components)
	{
		return SetSize(width,height,1,components);
	}

	bool YumeImage::SetSize(int width,int height,int depth,unsigned components)
	{
		if(width == width_ && height == height_ && depth == depth_ && components == components_)
			return true;

		if(width <= 0 || height <= 0 || depth <= 0)
			return false;

		if(components > 4)
		{
			YUMELOG_ERROR("More than 4 color components are not supported");
			return false;
		}

		data_ = boost::shared_array<unsigned char>(new unsigned char[width * height * depth * components]);
		width_ = width;
		height_ = height;
		depth_ = depth;
		components_ = components;
		compressedFormat_ = CF_NONE;
		numCompressedLevels_ = 0;
		nextLevel_.reset();

		SetMemoryUsage(width * height * depth * components);
		return true;
	}

	void YumeImage::SetPixel(int x,int y,const YumeColor& color)
	{
		SetPixelInt(x,y,0,color.ToUInt());
	}

	void YumeImage::SetPixel(int x,int y,int z,const YumeColor& color)
	{
		SetPixelInt(x,y,z,color.ToUInt());
	}

	void YumeImage::SetPixelInt(int x,int y,unsigned uintColor)
	{
		SetPixelInt(x,y,0,uintColor);
	}

	void YumeImage::SetPixelInt(int x,int y,int z,unsigned uintColor)
	{
		if(!data_ || x < 0 || x >= width_ || y < 0 || y >= height_ || z < 0 || z >= depth_ || IsCompressed())
			return;

		unsigned char* dest = data_.get() + (z * width_ * height_ + y * width_ + x) * components_;
		unsigned char* src = (unsigned char*)&uintColor;

		switch(components_)
		{
		case 4:
			dest[3] = src[3];
			// Fall through
		case 3:
			dest[2] = src[2];
			// Fall through
		case 2:
			dest[1] = src[1];
			// Fall through
		default:
			dest[0] = src[0];
			break;
		}
	}

	void YumeImage::SetData(const unsigned char* pixelData)
	{
		if(!data_)
			return;

		if(IsCompressed())
		{
			YUMELOG_ERROR("Can not set new pixel data for a compressed image");
			return;
		}

		memcpy(data_.get(),pixelData,width_ * height_ * depth_ * components_);
		nextLevel_.reset();
	}

	bool YumeImage::LoadColorLUT(YumeFile& source)
	{
		/*String fileID = source.ReadFileID();

		if(fileID == "DDS " || fileID == "\253KTX" || fileID == "PVR\3")
		{
		URHO3D_LOGERROR("Invalid image format, can not load image");
		return false;
		}

		source.Seek(0);
		int width,height;
		unsigned components;
		unsigned char* pixelDataIn = GetImageData(source,width,height,components);
		if(!pixelDataIn)
		{
		URHO3D_LOGERROR("Could not load image " + source.GetName() + ": " + String(stbi_failure_reason()));
		return false;
		}
		if(components != 3)
		{
		URHO3D_LOGERROR("Invalid image format, can not load image");
		return false;
		}

		SetSize(COLOR_LUT_SIZE,COLOR_LUT_SIZE,COLOR_LUT_SIZE,components);
		SetMemoryUse(width_ * height_ * depth_ * components);

		unsigned char* pixelDataOut = GetData();

		for(int z = 0; z < depth_; ++z)
		{
		for(int y = 0; y < height_; ++y)
		{
		unsigned char* in = &pixelDataIn[z * width_ * 3 + y * width * 3];
		unsigned char* out = &pixelDataOut[z * width_ * height_ * 3 + y * width_ * 3];

		for(int x = 0; x < width_ * 3; x += 3)
		{
		out[x] = in[x];
		out[x + 1] = in[x + 1];
		out[x + 2] = in[x + 2];
		}
		}
		}

		FreeImageData(pixelDataIn);
		*/
		return true;
	}

	bool YumeImage::FlipHorizontal()
	{
		if(!data_)
			return false;

		if(depth_ > 1)
		{
			YUMELOG_ERROR("FlipHorizontal not supported for 3D images");
			return false;
		}

		if(!IsCompressed())
		{
			boost::shared_array<unsigned char> newData(new unsigned char[width_ * height_ * components_]);
			unsigned rowSize = width_ * components_;

			for(int y = 0; y < height_; ++y)
			{
				for(int x = 0; x < width_; ++x)
				{
					for(unsigned c = 0; c < components_; ++c)
						newData[y * rowSize + x * components_ + c] = data_[y * rowSize + (width_ - x - 1) * components_ + c];
				}
			}

			data_ = newData;
		}
		else
		{
			if(compressedFormat_ > CF_DXT5)
			{
				YUMELOG_ERROR("FlipHorizontal not yet implemented for other compressed formats than RGBA & DXT1,3,5");
				return false;
			}

			// Memory use = combined size of the compressed mip levels
			boost::shared_array<unsigned char> newData(new unsigned char[GetMemoryUse()]);
			unsigned dataOffset = 0;

			for(unsigned i = 0; i < numCompressedLevels_; ++i)
			{
				CompressedLevel level = GetCompressedLevel(i);
				if(!level.data_)
				{
					YUMELOG_ERROR("Got compressed level with no data, aborting horizontal flip");
					return false;
				}

				for(unsigned y = 0; y < level.rows_; ++y)
				{
					for(unsigned x = 0; x < level.rowSize_; x += level.blockSize_)
					{
						unsigned char* src = level.data_ + y * level.rowSize_ + (level.rowSize_ - level.blockSize_ - x);
						unsigned char* dest = newData.get() + y * level.rowSize_ + x;
						FlipBlockHorizontal(dest,src,compressedFormat_);
					}
				}

				dataOffset += level.dataSize_;
			}

			data_ = newData;
		}

		return true;
	}

	bool YumeImage::FlipVertical()
	{
		if(!data_)
			return false;

		if(depth_ > 1)
		{
			YUMELOG_ERROR("FlipVertical not supported for 3D images");
			return false;
		}

		if(!IsCompressed())
		{
			boost::shared_array<unsigned char> newData(new unsigned char[width_ * height_ * components_]);
			unsigned rowSize = width_ * components_;

			for(int y = 0; y < height_; ++y)
				memcpy(&newData[(height_ - y - 1) * rowSize],&data_[y * rowSize],rowSize);

			data_ = newData;
		}
		else
		{
			if(compressedFormat_ > CF_DXT5)
			{
				YUMELOG_ERROR("FlipVertical not yet implemented for other compressed formats than DXT1,3,5");
				return false;
			}

			// Memory use = combined size of the compressed mip levels
			boost::shared_array<unsigned char> newData(new unsigned char[GetMemoryUse()]);
			unsigned dataOffset = 0;

			for(unsigned i = 0; i < numCompressedLevels_; ++i)
			{
				CompressedLevel level = GetCompressedLevel(i);
				if(!level.data_)
				{
					YUMELOG_ERROR("Got compressed level with no data, aborting vertical flip");
					return false;
				}

				for(unsigned y = 0; y < level.rows_; ++y)
				{
					unsigned char* src = level.data_ + y * level.rowSize_;
					unsigned char* dest = newData.get() + dataOffset + (level.rows_ - y - 1) * level.rowSize_;

					for(unsigned x = 0; x < level.rowSize_; x += level.blockSize_)
						FlipBlockVertical(dest + x,src + x,compressedFormat_);
				}

				dataOffset += level.dataSize_;
			}

			data_ = newData;
		}

		return true;
	}

	bool YumeImage::Resize(int width,int height)
	{

		if(IsCompressed())
		{
			YUMELOG_ERROR("Resize not supported for compressed images");
			return false;
		}

		if(depth_ > 1)
		{
			YUMELOG_ERROR("Resize not supported for 3D images");
			return false;
		}

		if(!data_ || width <= 0 || height <= 0)
			return false;


		boost::shared_array<unsigned char> newData(new unsigned char[width * height * components_]);
		for(int y = 0; y < height; ++y)
		{
			for(int x = 0; x < width; ++x)
			{
				// Calculate float coordinates between 0 - 1 for resampling
				float xF = (width_ > 1) ? (float)x / (float)(width - 1) : 0.0f;
				float yF = (height_ > 1) ? (float)y / (float)(height - 1) : 0.0f;
				unsigned uintColor = GetPixelBilinear(xF,yF).ToUInt();
				unsigned char* dest = newData.get() + (y * width + x) * components_;
				unsigned char* src = (unsigned char*)&uintColor;

				switch(components_)
				{
				case 4:
					dest[3] = src[3];
					// Fall through
				case 3:
					dest[2] = src[2];
					// Fall through
				case 2:
					dest[1] = src[1];
					// Fall through
				default:
					dest[0] = src[0];
					break;
				}
			}
		}

		width_ = width;
		height_ = height;
		data_ = newData;
		SetMemoryUsage(width * height * depth_ * components_);
		return true;
	}

	void YumeImage::Clear(const YumeColor& color)
	{
		ClearInt(color.ToUInt());
	}

	void YumeImage::ClearInt(unsigned uintColor)
	{
		if(!data_)
			return;

		if(IsCompressed())
		{
			YUMELOG_ERROR("Clear not supported for compressed images");
			return;
		}

		unsigned char* src = (unsigned char*)&uintColor;
		for(unsigned i = 0; i < width_ * height_ * depth_ * components_; ++i)
			data_[i] = src[i % components_];
	}

	bool YumeImage::SaveBMP(const YumeString& fileName) const
	{

		//FileSystem* fileSystem = GetSubsystem<FileSystem>();
		//if(fileSystem && !fileSystem->CheckAccess(GetPath(fileName)))
		//{
		//	YUMELOG_ERROR("Access denied to " + fileName);
		//	return false;
		//}

		//if(IsCompressed())
		//{
		//	YUMELOG_ERROR("Can not save compressed image to BMP");
		//	return false;
		//}

		if(data_)
			return stbi_write_bmp(fileName.c_str(),width_,height_,components_,data_.get()) != 0;
		else
			return false;
	}

	bool YumeImage::SavePNG(const YumeString& fileName) const
	{
		/*FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if(fileSystem && !fileSystem->CheckAccess(GetPath(fileName)))
		{
		URHO3D_LOGERROR("Access denied to " + fileName);
		return false;
		}

		if(IsCompressed())
		{
		URHO3D_LOGERROR("Can not save compressed image to PNG");
		return false;
		}
		*/
		if(data_)
			return stbi_write_png(fileName.c_str(),width_,height_,components_,data_.get(),0) != 0;
		else
			return false;
	}

	bool YumeImage::SaveTGA(const YumeString& fileName) const
	{
		/*URHO3D_PROFILE(SaveImageTGA);

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if(fileSystem && !fileSystem->CheckAccess(GetPath(fileName)))
		{
		URHO3D_LOGERROR("Access denied to " + fileName);
		return false;
		}

		if(IsCompressed())
		{
		URHO3D_LOGERROR("Can not save compressed image to TGA");
		return false;
		}*/

		if(data_)
			return stbi_write_tga((fileName).c_str(),width_,height_,components_,data_.get()) != 0;
		else
			return false;
	}

	bool YumeImage::SaveJPG(const YumeString& fileName,int quality) const
	{
		//FileSystem* fileSystem = GetSubsystem<FileSystem>();
		//if(fileSystem && !fileSystem->CheckAccess(GetPath(fileName)))
		//{
		//	YUMELOG_ERROR("Access denied to " + fileName);
		//	return false;
		//}

		//if(IsCompressed())
		//{
		//	YUMELOG_ERROR("Can not save compressed image to JPG");
		//	return false;
		//}

		if(data_)
			return jo_write_jpg((fileName).c_str(),data_.get(),width_,height_,components_,quality) != 0;
		else
			return false;
	}

	YumeColor YumeImage::GetPixel(int x,int y) const
	{
		return GetPixel(x,y,0);
	}

	YumeColor YumeImage::GetPixel(int x,int y,int z) const
	{
		if(!data_ || z < 0 || z >= depth_ || IsCompressed())
			return YumeColor::BLACK;
		x = Math::Clamp(x,0,width_ - 1);
		y = Math::Clamp(y,0,height_ - 1);

		unsigned char* src = data_.get() + (z * width_ * height_ + y * width_ + x) * components_;
		YumeColor ret;

		switch(components_)
		{
		case 4:
			ret.a_ = (float)src[3] / 255.0f;
			// Fall through
		case 3:
			ret.b_ = (float)src[2] / 255.0f;
			// Fall through
		case 2:
			ret.g_ = (float)src[1] / 255.0f;
			ret.r_ = (float)src[0] / 255.0f;
			break;
		default:
			ret.r_ = ret.g_ = ret.b_ = (float)src[0] / 255.0f;
			break;
		}

		return ret;
	}

	unsigned YumeImage::GetPixelInt(int x,int y) const
	{
		return GetPixelInt(x,y,0);
	}

	unsigned YumeImage::GetPixelInt(int x,int y,int z) const
	{
		if(!data_ || z < 0 || z >= depth_ || IsCompressed())
			return 0xff000000;
		x = Math::Clamp(x,0,width_ - 1);
		y = Math::Clamp(y,0,height_ - 1);

		unsigned char* src = data_.get() + (z * width_ * height_ + y * width_ + x) * components_;
		unsigned ret = 0;
		if(components_ < 4)
			ret |= 0xff000000;

		switch(components_)
		{
		case 4:
			ret |= (unsigned)src[3] << 24;
			// Fall through
		case 3:
			ret |= (unsigned)src[2] << 16;
			// Fall through
		case 2:
			ret |= (unsigned)src[1] << 8;
			ret |= (unsigned)src[0];
			break;
		default:
			ret |= (unsigned)src[0] << 16;
			ret |= (unsigned)src[0] << 8;
			ret |= (unsigned)src[0];
			break;
		}

		return ret;
	}

	YumeColor YumeImage::GetPixelBilinear(float x,float y) const
	{
		x = Math::Clamp(x * width_ - 0.5f,0.0f,(float)(width_ - 1));
		y = Math::Clamp(y * height_ - 0.5f,0.0f,(float)(height_ - 1));

		int xI = (int)x;
		int yI = (int)y;
		float xF = x - floorf(x);
		float yF = y - floorf(y);

		YumeColor topColor = GetPixel(xI,yI).Lerp(GetPixel(xI + 1,yI),xF);
		YumeColor bottomColor = GetPixel(xI,yI + 1).Lerp(GetPixel(xI + 1,yI + 1),xF);
		return topColor.Lerp(bottomColor,yF);
	}

	YumeColor YumeImage::GetPixelTrilinear(float x,float y,float z) const
	{
		if(depth_ < 2)
			return GetPixelBilinear(x,y);

		x = Math::Clamp(x * width_ - 0.5f,0.0f,(float)(width_ - 1));
		y = Math::Clamp(y * height_ - 0.5f,0.0f,(float)(height_ - 1));
		z = Math::Clamp(z * depth_ - 0.5f,0.0f,(float)(depth_ - 1));

		int xI = (int)x;
		int yI = (int)y;
		int zI = (int)z;
		if(zI == depth_ - 1)
			return GetPixelBilinear(x,y);
		float xF = x - floorf(x);
		float yF = y - floorf(y);
		float zF = z - floorf(z);

		YumeColor topColorNear = GetPixel(xI,yI,zI).Lerp(GetPixel(xI + 1,yI,zI),xF);
		YumeColor bottomColorNear = GetPixel(xI,yI + 1,zI).Lerp(GetPixel(xI + 1,yI + 1,zI),xF);
		YumeColor colorNear = topColorNear.Lerp(bottomColorNear,yF);
		YumeColor topColorFar = GetPixel(xI,yI,zI + 1).Lerp(GetPixel(xI + 1,yI,zI + 1),xF);
		YumeColor bottomColorFar = GetPixel(xI,yI + 1,zI + 1).Lerp(GetPixel(xI + 1,yI + 1,zI + 1),xF);
		YumeColor colorFar = topColorFar.Lerp(bottomColorFar,yF);
		return colorNear.Lerp(colorFar,zF);
	}

	boost::shared_ptr<YumeImage> YumeImage::GetNextLevel() const
	{
		if(IsCompressed())
		{
			YUMELOG_ERROR("Can not generate mip level from compressed data");
			return boost::shared_ptr<YumeImage>();
		}
		if(components_ < 1 || components_ > 4)
		{
			YUMELOG_ERROR("Illegal number of image components for mip level generation");
			return boost::shared_ptr<YumeImage>();
		}

		if(nextLevel_)
			return nextLevel_;

		int widthOut = width_ / 2;
		int heightOut = height_ / 2;
		int depthOut = depth_ / 2;

		if(widthOut < 1)
			widthOut = 1;
		if(heightOut < 1)
			heightOut = 1;
		if(depthOut < 1)
			depthOut = 1;

		boost::shared_ptr<YumeImage> mipImage(new YumeImage());

		if(depth_ > 1)
			mipImage->SetSize(widthOut,heightOut,depthOut,components_);
		else
			mipImage->SetSize(widthOut,heightOut,components_);

		const unsigned char* pixelDataIn = data_.get();
		unsigned char* pixelDataOut = mipImage->data_.get();

		// 1D case
		if(depth_ == 1 && (height_ == 1 || width_ == 1))
		{
			// Loop using the larger dimension
			if(widthOut < heightOut)
				widthOut = heightOut;

			switch(components_)
			{
			case 1:
				for(int x = 0; x < widthOut; ++x)
					pixelDataOut[x] = (unsigned char)(((unsigned)pixelDataIn[x * 2] + pixelDataIn[x * 2 + 1]) >> 1);
				break;

			case 2:
				for(int x = 0; x < widthOut * 2; x += 2)
				{
					pixelDataOut[x] = (unsigned char)(((unsigned)pixelDataIn[x * 2] + pixelDataIn[x * 2 + 2]) >> 1);
					pixelDataOut[x + 1] = (unsigned char)(((unsigned)pixelDataIn[x * 2 + 1] + pixelDataIn[x * 2 + 3]) >> 1);
				}
				break;

			case 3:
				for(int x = 0; x < widthOut * 3; x += 3)
				{
					pixelDataOut[x] = (unsigned char)(((unsigned)pixelDataIn[x * 2] + pixelDataIn[x * 2 + 3]) >> 1);
					pixelDataOut[x + 1] = (unsigned char)(((unsigned)pixelDataIn[x * 2 + 1] + pixelDataIn[x * 2 + 4]) >> 1);
					pixelDataOut[x + 2] = (unsigned char)(((unsigned)pixelDataIn[x * 2 + 2] + pixelDataIn[x * 2 + 5]) >> 1);
				}
				break;

			case 4:
				for(int x = 0; x < widthOut * 4; x += 4)
				{
					pixelDataOut[x] = (unsigned char)(((unsigned)pixelDataIn[x * 2] + pixelDataIn[x * 2 + 4]) >> 1);
					pixelDataOut[x + 1] = (unsigned char)(((unsigned)pixelDataIn[x * 2 + 1] + pixelDataIn[x * 2 + 5]) >> 1);
					pixelDataOut[x + 2] = (unsigned char)(((unsigned)pixelDataIn[x * 2 + 2] + pixelDataIn[x * 2 + 6]) >> 1);
					pixelDataOut[x + 3] = (unsigned char)(((unsigned)pixelDataIn[x * 2 + 3] + pixelDataIn[x * 2 + 7]) >> 1);
				}
				break;

			default:
				assert(false);  // Should never reach here
				break;
			}
		}
		// 2D case
		else if(depth_ == 1)
		{
			switch(components_)
			{
			case 1:
				for(int y = 0; y < heightOut; ++y)
				{
					const unsigned char* inUpper = &pixelDataIn[(y * 2) * width_];
					const unsigned char* inLower = &pixelDataIn[(y * 2 + 1) * width_];
					unsigned char* out = &pixelDataOut[y * widthOut];

					for(int x = 0; x < widthOut; ++x)
					{
						out[x] = (unsigned char)(((unsigned)inUpper[x * 2] + inUpper[x * 2 + 1] +
							inLower[x * 2] + inLower[x * 2 + 1]) >> 2);
					}
				}
				break;

			case 2:
				for(int y = 0; y < heightOut; ++y)
				{
					const unsigned char* inUpper = &pixelDataIn[(y * 2) * width_ * 2];
					const unsigned char* inLower = &pixelDataIn[(y * 2 + 1) * width_ * 2];
					unsigned char* out = &pixelDataOut[y * widthOut * 2];

					for(int x = 0; x < widthOut * 2; x += 2)
					{
						out[x] = (unsigned char)(((unsigned)inUpper[x * 2] + inUpper[x * 2 + 2] +
							inLower[x * 2] + inLower[x * 2 + 2]) >> 2);
						out[x + 1] = (unsigned char)(((unsigned)inUpper[x * 2 + 1] + inUpper[x * 2 + 3] +
							inLower[x * 2 + 1] + inLower[x * 2 + 3]) >> 2);
					}
				}
				break;

			case 3:
				for(int y = 0; y < heightOut; ++y)
				{
					const unsigned char* inUpper = &pixelDataIn[(y * 2) * width_ * 3];
					const unsigned char* inLower = &pixelDataIn[(y * 2 + 1) * width_ * 3];
					unsigned char* out = &pixelDataOut[y * widthOut * 3];

					for(int x = 0; x < widthOut * 3; x += 3)
					{
						out[x] = (unsigned char)(((unsigned)inUpper[x * 2] + inUpper[x * 2 + 3] +
							inLower[x * 2] + inLower[x * 2 + 3]) >> 2);
						out[x + 1] = (unsigned char)(((unsigned)inUpper[x * 2 + 1] + inUpper[x * 2 + 4] +
							inLower[x * 2 + 1] + inLower[x * 2 + 4]) >> 2);
						out[x + 2] = (unsigned char)(((unsigned)inUpper[x * 2 + 2] + inUpper[x * 2 + 5] +
							inLower[x * 2 + 2] + inLower[x * 2 + 5]) >> 2);
					}
				}
				break;

			case 4:
				for(int y = 0; y < heightOut; ++y)
				{
					const unsigned char* inUpper = &pixelDataIn[(y * 2) * width_ * 4];
					const unsigned char* inLower = &pixelDataIn[(y * 2 + 1) * width_ * 4];
					unsigned char* out = &pixelDataOut[y * widthOut * 4];

					for(int x = 0; x < widthOut * 4; x += 4)
					{
						out[x] = (unsigned char)(((unsigned)inUpper[x * 2] + inUpper[x * 2 + 4] +
							inLower[x * 2] + inLower[x * 2 + 4]) >> 2);
						out[x + 1] = (unsigned char)(((unsigned)inUpper[x * 2 + 1] + inUpper[x * 2 + 5] +
							inLower[x * 2 + 1] + inLower[x * 2 + 5]) >> 2);
						out[x + 2] = (unsigned char)(((unsigned)inUpper[x * 2 + 2] + inUpper[x * 2 + 6] +
							inLower[x * 2 + 2] + inLower[x * 2 + 6]) >> 2);
						out[x + 3] = (unsigned char)(((unsigned)inUpper[x * 2 + 3] + inUpper[x * 2 + 7] +
							inLower[x * 2 + 3] + inLower[x * 2 + 7]) >> 2);
					}
				}
				break;

			default:
				assert(false);  // Should never reach here
				break;
			}
		}
		// 3D case
		else
		{
			switch(components_)
			{
			case 1:
				for(int z = 0; z < depthOut; ++z)
				{
					const unsigned char* inOuter = &pixelDataIn[(z * 2) * width_ * height_];
					const unsigned char* inInner = &pixelDataIn[(z * 2 + 1) * width_ * height_];

					for(int y = 0; y < heightOut; ++y)
					{
						const unsigned char* inOuterUpper = &inOuter[(y * 2) * width_];
						const unsigned char* inOuterLower = &inOuter[(y * 2 + 1) * width_];
						const unsigned char* inInnerUpper = &inInner[(y * 2) * width_];
						const unsigned char* inInnerLower = &inInner[(y * 2 + 1) * width_];
						unsigned char* out = &pixelDataOut[z * widthOut * heightOut + y * widthOut];

						for(int x = 0; x < widthOut; ++x)
						{
							out[x] = (unsigned char)(((unsigned)inOuterUpper[x * 2] + inOuterUpper[x * 2 + 1] +
								inOuterLower[x * 2] + inOuterLower[x * 2 + 1] +
								inInnerUpper[x * 2] + inInnerUpper[x * 2 + 1] +
								inInnerLower[x * 2] + inInnerLower[x * 2 + 1]) >> 3);
						}
					}
				}
				break;

			case 2:
				for(int z = 0; z < depthOut; ++z)
				{
					const unsigned char* inOuter = &pixelDataIn[(z * 2) * width_ * height_ * 2];
					const unsigned char* inInner = &pixelDataIn[(z * 2 + 1) * width_ * height_ * 2];

					for(int y = 0; y < heightOut; ++y)
					{
						const unsigned char* inOuterUpper = &inOuter[(y * 2) * width_ * 2];
						const unsigned char* inOuterLower = &inOuter[(y * 2 + 1) * width_ * 2];
						const unsigned char* inInnerUpper = &inInner[(y * 2) * width_ * 2];
						const unsigned char* inInnerLower = &inInner[(y * 2 + 1) * width_ * 2];
						unsigned char* out = &pixelDataOut[z * widthOut * heightOut * 2 + y * widthOut * 2];

						for(int x = 0; x < widthOut * 2; x += 2)
						{
							out[x] = (unsigned char)(((unsigned)inOuterUpper[x * 2] + inOuterUpper[x * 2 + 2] +
								inOuterLower[x * 2] + inOuterLower[x * 2 + 2] +
								inInnerUpper[x * 2] + inInnerUpper[x * 2 + 2] +
								inInnerLower[x * 2] + inInnerLower[x * 2 + 2]) >> 3);
							out[x + 1] = (unsigned char)(((unsigned)inOuterUpper[x * 2 + 1] + inOuterUpper[x * 2 + 3] +
								inOuterLower[x * 2 + 1] + inOuterLower[x * 2 + 3] +
								inInnerUpper[x * 2 + 1] + inInnerUpper[x * 2 + 3] +
								inInnerLower[x * 2 + 1] + inInnerLower[x * 2 + 3]) >> 3);
						}
					}
				}
				break;

			case 3:
				for(int z = 0; z < depthOut; ++z)
				{
					const unsigned char* inOuter = &pixelDataIn[(z * 2) * width_ * height_ * 3];
					const unsigned char* inInner = &pixelDataIn[(z * 2 + 1) * width_ * height_ * 3];

					for(int y = 0; y < heightOut; ++y)
					{
						const unsigned char* inOuterUpper = &inOuter[(y * 2) * width_ * 3];
						const unsigned char* inOuterLower = &inOuter[(y * 2 + 1) * width_ * 3];
						const unsigned char* inInnerUpper = &inInner[(y * 2) * width_ * 3];
						const unsigned char* inInnerLower = &inInner[(y * 2 + 1) * width_ * 3];
						unsigned char* out = &pixelDataOut[z * widthOut * heightOut * 3 + y * widthOut * 3];

						for(int x = 0; x < widthOut * 3; x += 3)
						{
							out[x] = (unsigned char)(((unsigned)inOuterUpper[x * 2] + inOuterUpper[x * 2 + 3] +
								inOuterLower[x * 2] + inOuterLower[x * 2 + 3] +
								inInnerUpper[x * 2] + inInnerUpper[x * 2 + 3] +
								inInnerLower[x * 2] + inInnerLower[x * 2 + 3]) >> 3);
							out[x + 1] = (unsigned char)(((unsigned)inOuterUpper[x * 2 + 1] + inOuterUpper[x * 2 + 4] +
								inOuterLower[x * 2 + 1] + inOuterLower[x * 2 + 4] +
								inInnerUpper[x * 2 + 1] + inInnerUpper[x * 2 + 4] +
								inInnerLower[x * 2 + 1] + inInnerLower[x * 2 + 4]) >> 3);
							out[x + 2] = (unsigned char)(((unsigned)inOuterUpper[x * 2 + 2] + inOuterUpper[x * 2 + 5] +
								inOuterLower[x * 2 + 2] + inOuterLower[x * 2 + 5] +
								inInnerUpper[x * 2 + 2] + inInnerUpper[x * 2 + 5] +
								inInnerLower[x * 2 + 2] + inInnerLower[x * 2 + 5]) >> 3);
						}
					}
				}
				break;

			case 4:
				for(int z = 0; z < depthOut; ++z)
				{
					const unsigned char* inOuter = &pixelDataIn[(z * 2) * width_ * height_ * 4];
					const unsigned char* inInner = &pixelDataIn[(z * 2 + 1) * width_ * height_ * 4];

					for(int y = 0; y < heightOut; ++y)
					{
						const unsigned char* inOuterUpper = &inOuter[(y * 2) * width_ * 4];
						const unsigned char* inOuterLower = &inOuter[(y * 2 + 1) * width_ * 4];
						const unsigned char* inInnerUpper = &inInner[(y * 2) * width_ * 4];
						const unsigned char* inInnerLower = &inInner[(y * 2 + 1) * width_ * 4];
						unsigned char* out = &pixelDataOut[z * widthOut * heightOut * 4 + y * widthOut * 4];

						for(int x = 0; x < widthOut * 4; x += 4)
						{
							out[x] = (unsigned char)(((unsigned)inOuterUpper[x * 2] + inOuterUpper[x * 2 + 4] +
								inOuterLower[x * 2] + inOuterLower[x * 2 + 4] +
								inInnerUpper[x * 2] + inInnerUpper[x * 2 + 4] +
								inInnerLower[x * 2] + inInnerLower[x * 2 + 4]) >> 3);
							out[x + 1] = (unsigned char)(((unsigned)inOuterUpper[x * 2 + 1] + inOuterUpper[x * 2 + 5] +
								inOuterLower[x * 2 + 1] + inOuterLower[x * 2 + 5] +
								inInnerUpper[x * 2 + 1] + inInnerUpper[x * 2 + 5] +
								inInnerLower[x * 2 + 1] + inInnerLower[x * 2 + 5]) >> 3);
							out[x + 2] = (unsigned char)(((unsigned)inOuterUpper[x * 2 + 2] + inOuterUpper[x * 2 + 6] +
								inOuterLower[x * 2 + 2] + inOuterLower[x * 2 + 6] +
								inInnerUpper[x * 2 + 2] + inInnerUpper[x * 2 + 6] +
								inInnerLower[x * 2 + 2] + inInnerLower[x * 2 + 6]) >> 3);
						}
					}
				}
				break;

			default:
				assert(false);  // Should never reach here
				break;
			}
		}

		return mipImage;
	}

	boost::shared_ptr<YumeImage> YumeImage::ConvertToRGBA() const
	{
		if(IsCompressed())
		{
			YUMELOG_ERROR("Can not convert compressed image to RGBA");
			return boost::shared_ptr<YumeImage>();
		}
		if(components_ < 1 || components_ > 4)
		{
			YUMELOG_ERROR("Illegal number of image components for conversion to RGBA");
			return boost::shared_ptr<YumeImage>();
		}
		if(!data_)
		{
			YUMELOG_ERROR("Can not convert image without data to RGBA");
			return boost::shared_ptr<YumeImage>();
		}

		// Already RGBA?
		if(components_ == 4)
			return boost::shared_ptr<YumeImage>(const_cast<YumeImage*>(this));

		boost::shared_ptr<YumeImage> ret(new YumeImage());
		ret->SetSize(width_,height_,depth_,4);

		const unsigned char* src = data_.get();
		unsigned char* dest = ret->GetData();

		switch(components_)
		{
		case 1:
			for(unsigned i = 0; i < static_cast<unsigned>(width_ * height_ * depth_); ++i)
			{
				unsigned char pixel = *src++;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = 255;
			}
			break;

		case 2:
			for(unsigned i = 0; i < static_cast<unsigned>(width_ * height_ * depth_); ++i)
			{
				unsigned char pixel = *src++;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = *src++;
			}
			break;

		case 3:
			for(unsigned i = 0; i < static_cast<unsigned>(width_ * height_ * depth_); ++i)
			{
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = 255;
			}
			break;

		default:
			assert(false);  // Should never reach nere
			break;
		}

		return ret;
	}

	CompressedLevel YumeImage::GetCompressedLevel(unsigned index) const
	{
		CompressedLevel level;

		if(compressedFormat_ == CF_NONE)
		{
			YUMELOG_ERROR("Image is not compressed");
			return level;
		}
		if(index >= numCompressedLevels_)
		{
			YUMELOG_ERROR("Compressed image mip level out of bounds");
			return level;
		}

		level.format_ = compressedFormat_;
		level.width_ = width_;
		level.height_ = height_;
		level.depth_ = depth_;

		if(compressedFormat_ == CF_RGBA)
		{
			level.blockSize_ = 4;
			unsigned i = 0;
			unsigned offset = 0;

			for(;;)
			{
				if(!level.width_)
					level.width_ = 1;
				if(!level.height_)
					level.height_ = 1;
				if(!level.depth_)
					level.depth_ = 1;

				level.rowSize_ = level.width_ * level.blockSize_;
				level.rows_ = (unsigned)level.height_;
				level.data_ = data_.get() + offset;
				level.dataSize_ = level.depth_ * level.rows_ * level.rowSize_;

				if(offset + level.dataSize_ > GetMemoryUse())
				{
					YUMELOG_ERROR("Compressed level is outside image data. Offset: " << std::to_string(offset) << " Size: " << std::to_string(level.dataSize_) << " " +std::to_string(GetMemoryUse()));

					level.data_ = 0;
					return level;
				}

				if(i == index)
					return level;

				offset += level.dataSize_;
				level.width_ /= 2;
				level.height_ /= 2;
				level.depth_ /= 2;
				++i;
			}
		}
		else if(compressedFormat_ < CF_PVRTC_RGB_2BPP)
		{
			level.blockSize_ = (compressedFormat_ == CF_DXT1 || compressedFormat_ == CF_ETC1) ? 8 : 16;
			unsigned i = 0;
			unsigned offset = 0;

			for(;;)
			{
				if(!level.width_)
					level.width_ = 1;
				if(!level.height_)
					level.height_ = 1;
				if(!level.depth_)
					level.depth_ = 1;

				level.rowSize_ = ((level.width_ + 3) / 4) * level.blockSize_;
				level.rows_ = (unsigned)((level.height_ + 3) / 4);
				level.data_ = data_.get() + offset;
				level.dataSize_ = level.depth_ * level.rows_ * level.rowSize_;

				if(offset + level.dataSize_ > GetMemoryUse())
				{
					YUMELOG_ERROR("Compressed level is outside image data. Offset: " + std::to_string(offset) + " Size: " + std::to_string(level.dataSize_) +
						" Datasize: " + std::to_string(GetMemoryUse()));
					level.data_ = 0;
					return level;
				}

				if(i == index)
					return level;

				offset += level.dataSize_;
				level.width_ /= 2;
				level.height_ /= 2;
				level.depth_ /= 2;
				++i;
			}
		}
		else
		{
			level.blockSize_ = compressedFormat_ < CF_PVRTC_RGB_4BPP ? 2 : 4;
			unsigned i = 0;
			unsigned offset = 0;

			for(;;)
			{
				if(!level.width_)
					level.width_ = 1;
				if(!level.height_)
					level.height_ = 1;

				int dataWidth = std::max(level.width_,level.blockSize_ == 2 ? 16 : 8);
				int dataHeight = std::max(level.height_,8);
				level.data_ = data_.get() + offset;
				level.dataSize_ = (dataWidth * dataHeight * level.blockSize_ + 7) >> 3;
				level.rows_ = (unsigned)dataHeight;
				level.rowSize_ = level.dataSize_ / level.rows_;

				if(offset + level.dataSize_ > GetMemoryUse())
				{
					YUMELOG_ERROR("Compressed level is outside image data. Offset: " + std::to_string(offset) + " Size: " + std::to_string(level.dataSize_) +
						" Datasize: " + std::to_string(GetMemoryUse()));
					level.data_ = 0;
					return level;
				}

				if(i == index)
					return level;

				offset += level.dataSize_;
				level.width_ /= 2;
				level.height_ /= 2;
				++i;
			}
		}
	}

	YumeImage* YumeImage::GetSubimage(const IntRect& rect) const
	{
		if(!data_)
			return 0;

		if(depth_ > 1)
		{
			YUMELOG_ERROR("Subimage not supported for 3D images");
			return 0;
		}

		if(rect.left_ < 0 || rect.top_ < 0 || rect.right_ > width_ || rect.bottom_ > height_ || !rect.Width() || !rect.Height())
		{
			YUMELOG_ERROR("Can not get subimage from image " + GetName() + " with invalid region");
			return 0;
		}

		if(!IsCompressed())
		{
			int x = rect.left_;
			int y = rect.top_;
			int width = rect.Width();
			int height = rect.Height();

			YumeImage* image = new YumeImage;
			image->SetSize(width,height,components_);

			unsigned char* dest = image->GetData();
			unsigned char* src = data_.get() + (y * width_ + x) * components_;
			for(int i = 0; i < height; ++i)
			{
				memcpy(dest,src,width * components_);
				dest += width * components_;
				src += width_ * components_;
			}

			return image;
		}
		else
		{
			// Pad the region to be a multiple of block size
			IntRect paddedRect = rect;
			paddedRect.left_ = (rect.left_ / 4) * 4;
			paddedRect.top_ = (rect.top_ / 4) * 4;
			paddedRect.right_ = (rect.right_ / 4) * 4;
			paddedRect.bottom_ = (rect.bottom_ / 4) * 4;
			IntRect currentRect = paddedRect;

			YumeVector<unsigned char>::type subimageData;
			unsigned subimageLevels = 0;

			// Save as many mips as possible until the next mip would cross a block boundary
			for(unsigned i = 0; i < numCompressedLevels_; ++i)
			{
				CompressedLevel level = GetCompressedLevel(i);
				if(!level.data_)
					break;

				// Mips are stored continuously
				unsigned destStartOffset = subimageData.size();
				unsigned destRowSize = currentRect.Width() / 4 * level.blockSize_;
				unsigned destSize = currentRect.Height() / 4 * destRowSize;
				if(!destSize)
					break;

				subimageData.resize(destStartOffset + destSize);
				unsigned char* dest = &subimageData[destStartOffset];

				for(int y = currentRect.top_; y < currentRect.bottom_; y += 4)
				{
					unsigned char* src = level.data_ + level.rowSize_ * (y / 4) + currentRect.left_ / 4 * level.blockSize_;
					memcpy(dest,src,destRowSize);
					dest += destRowSize;
				}

				++subimageLevels;
				if((currentRect.left_ & 4) || (currentRect.right_ & 4) || (currentRect.top_ & 4) || (currentRect.bottom_ & 4))
					break;
				else
				{
					currentRect.left_ /= 2;
					currentRect.right_ /= 2;
					currentRect.top_ /= 2;
					currentRect.bottom_ /= 2;
				}
			}

			if(!subimageLevels)
			{
				YUMELOG_ERROR("Subimage region from compressed image " + GetName() + " did not produce any data");
				return 0;
			}

			YumeImage* image = new YumeImage;
			image->width_ = paddedRect.Width();
			image->height_ = paddedRect.Height();
			image->depth_ = 1;
			image->compressedFormat_ = compressedFormat_;
			image->numCompressedLevels_ = subimageLevels;
			image->components_ = components_;
			image->data_ = boost::shared_array<unsigned char>(new unsigned char[subimageData.size()]);
			memcpy(image->data_.get(),&subimageData[0],subimageData.size());
			image->SetMemoryUsage(subimageData.size());

			return image;
		}
	}

	SDL_Surface* YumeImage::GetSDLSurface(const IntRect& rect) const
	{
		if(!data_)
			return 0;

		if(depth_ > 1)
		{
			YUMELOG_ERROR("Can not get SDL surface from 3D image");
			return 0;
		}

		if(IsCompressed())
		{
			YUMELOG_ERROR("Can not get SDL surface from compressed image " + GetName());
			return 0;
		}

		if(components_ < 3)
		{
			YUMELOG_ERROR("Can not get SDL surface from image " << GetName().c_str() << " with less than 3 components");
			return 0;
		}

		IntRect imageRect = rect;
		// Use full image if illegal rect
		if(imageRect.left_ < 0 || imageRect.top_ < 0 || imageRect.right_ > width_ || imageRect.bottom_ > height_ ||
			imageRect.left_ >= imageRect.right_ || imageRect.top_ >= imageRect.bottom_)
		{
			imageRect.left_ = 0;
			imageRect.top_ = 0;
			imageRect.right_ = width_;
			imageRect.bottom_ = height_;
		}

		int imageWidth = width_;
		int width = imageRect.Width();
		int height = imageRect.Height();

		// Assume little-endian for all the supported platforms
		unsigned rMask = 0x000000ff;
		unsigned gMask = 0x0000ff00;
		unsigned bMask = 0x00ff0000;
		unsigned aMask = 0xff000000;

		SDL_Surface* surface = SDL_CreateRGBSurface(0,width,height,components_ * 8,rMask,gMask,bMask,aMask);
		if(surface)
		{
			SDL_LockSurface(surface);

			unsigned char* destination = reinterpret_cast<unsigned char*>(surface->pixels);
			unsigned char* source = data_.get() + components_ * (imageWidth * imageRect.top_ + imageRect.left_);
			for(int i = 0; i < height; ++i)
			{
				memcpy(destination,source,components_ * width);
				destination += surface->pitch;
				source += components_ * imageWidth;
			}

			SDL_UnlockSurface(surface);
		}
		else
			YUMELOG_ERROR("Failed to create SDL surface from image " + GetName());

		return surface;
	}

	void YumeImage::PrecalculateLevels()
	{
		if(!data_ || IsCompressed())
			return;

		nextLevel_.reset();

		if(width_ > 1 || height_ > 1)
		{
			boost::shared_ptr<YumeImage> current = GetNextLevel();
			nextLevel_ = current;
			while(current && (current->width_ > 1 || current->height_ > 1))
			{
				current->nextLevel_ = current->GetNextLevel();
				current = current->nextLevel_;
			}
		}
	}

	unsigned char* YumeImage::GetImageData(YumeFile& source,int& width,int& height,unsigned& components)
	{
		unsigned dataSize = source.GetSize();

		boost::shared_array<unsigned char> buffer(new unsigned char[dataSize]);
		source.Read(buffer.get(),dataSize);
		return stbi_load_from_memory(buffer.get(),dataSize,&width,&height,(int*)&components,0);
	}

	void YumeImage::FreeImageData(unsigned char* pixelData)
	{
		if(!pixelData)
			return;

		stbi_image_free(pixelData);
	}
}
