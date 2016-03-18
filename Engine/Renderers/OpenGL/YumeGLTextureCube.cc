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
#include "YumeGLTexture2D.h"
#include "YumeGLTextureCube.h"

#include "YumeGLRenderer.h"
#include "Renderer/YumeRHI.h"

#include "YumeGLRenderer.h"
#include "YumeGLRendererImpl.h"
#include "YumeGLRenderable.h"

#include "Renderer/YumeImage.h"
#include "Renderer/YumeRenderer.h"
#include "Renderer/YumeRenderable.h"
#include "Renderer/YumeResourceManager.h"

#include "Engine/YumeEngine.h"

#include "Logging/logging.h"



namespace YumeEngine
{
	static GLenum glWrapModes[] =
	{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP
	};

	static GLenum gl3WrapModes[] =
	{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER
	};

	static const char* addressModeNames[] =
	{
		"wrap",
		"mirror",
		"clamp",
		"border",
		0
	};

	static const char* filterModeNames[] =
	{
		"nearest",
		"bilinear",
		"trilinear",
		"anisotropic",
		"default",
		0
	};

	static GLenum GetWrapMode(TextureAddressMode mode)
	{
		return YumeGLRenderer::GetGL3Support() ? gl3WrapModes[mode] : glWrapModes[mode];
	}

	static const char* cubeMapLayoutNames[] ={
		"horizontal",
		"horizontalnvidia",
		"horizontalcross",
		"verticalcross",
		"blender",
		0
	};

	static SharedPtr<YumeImage> GetTileImage(YumeImage* src,int tileX,int tileY,int tileWidth,int tileHeight)
	{
		return SharedPtr<YumeImage>(
			src->GetSubimage(IntRect(tileX * tileWidth,tileY * tileHeight,(tileX + 1) * tileWidth,(tileY + 1) * tileHeight)));
	}

	YumeGLTextureCube::YumeGLTextureCube()
	{
		target_ = GL_TEXTURE_CUBE_MAP;
	}

	YumeGLTextureCube::~YumeGLTextureCube()
	{
		Release();
	}


	void YumeGLTextureCube::OnDeviceLost()
	{
		YumeGLResource::OnDeviceLost();

		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			if(renderSurfaces_[i])
				static_cast<YumeGLRenderable*>(renderSurfaces_[i].get())->OnDeviceLost();
		}
	}

	void YumeGLTextureCube::OnDeviceReset()
	{
		if(!object_ || dataPending_)
		{
			// If has a resource file, reload through the resource cache. Otherwise just recreate.
			YumeResourceManager* cache = gYume->pResourceManager;
			if(cache->Exists(GetName()))
				dataLost_ = !cache->ReloadResource(this);

			if(!object_)
			{
				Create();
				dataLost_ = true;
			}
		}

		dataPending_ = false;
	}

	bool YumeGLTextureCube::IsDataLost()
	{
		return YumeGLResource::IsDataLost();
	}
	void YumeGLTextureCube::ClearDataLost()
	{
		YumeGLResource::ClearDataLost();
	}

	void YumeGLTextureCube::Release()
	{
		if(object_)
		{
			if(!gYume->pRHI)
				return;

			if(!gYume->pRHI->IsDeviceLost())
			{
				for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
				{
					if(gYume->pRHI->GetTexture(i) == this)
						gYume->pRHI->SetTexture(i,0);
				}

				glDeleteTextures(1,&object_);
			}

			for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
			{
				if(renderSurfaces_[i])
					renderSurfaces_[i]->Release();
			}

			object_ = 0;
		}
	}

	bool YumeGLTextureCube::SetSize(int size,unsigned format,TextureUsage usage)
	{
		if(size <= 0)
		{
			YUMELOG_ERROR("Zero or negative cube texture size");
			return false;
		}
		if(usage == TEXTURE_DEPTHSTENCIL)
		{
			YUMELOG_ERROR("Depth-stencil usage not supported for cube maps");
			return false;
		}

		// Delete the old rendersurfaces if any
		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			renderSurfaces_[i].reset();
			faceMemoryUse_[i] = 0;
		}

		usage_ = usage;

		if(usage == TEXTURE_RENDERTARGET)
		{
			for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
			{
				renderSurfaces_[i] = SharedPtr<YumeGLRenderable>(new YumeGLRenderable(this));
				static_cast<YumeGLRenderable*>(renderSurfaces_[i].get())->SetTarget(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}

			// Nearest filtering and mipmaps disabled by default
			filterMode_ = FILTER_NEAREST;
			requestedLevels_ = 1;
		}

		if(usage == TEXTURE_RENDERTARGET)
			gYume->pRenderer->AddListener(this);
		else
			gYume->pRenderer->RemoveListener(this);

		width_ = size;
		height_ = size;
		format_ = format;

		return Create();
	}

	bool YumeGLTextureCube::SetData(CubeMapFace face,unsigned level,int x,int y,int width,int height,const void* data)
	{
		if(!object_)
		{
			YUMELOG_ERROR("No texture created, can not set data");
			return false;
		}

		if(!data)
		{
			YUMELOG_ERROR("Null source for setting data");
			return false;
		}

		if(level >= levels_)
		{
			YUMELOG_ERROR("Illegal mip level for setting data");
			return false;
		}

		if(gYume->pRHI->IsDeviceLost())
		{
			YUMELOG_WARN("Texture data assignment while device is lost");
			dataPending_ = true;
			return true;
		}

		if(IsCompressed())
		{
			x &= ~3;
			y &= ~3;
		}

		int levelWidth = GetLevelWidth(level);
		int levelHeight = GetLevelHeight(level);
		if(x < 0 || x + width > levelWidth || y < 0 || y + height > levelHeight || width <= 0 || height <= 0)
		{
			YUMELOG_ERROR("Illegal dimensions for setting data");
			return false;
		}

		static_cast<YumeGLRenderer*>(gYume->pRHI)->SetTextureForUpdate(this);

		bool wholeLevel = x == 0 && y == 0 && width == levelWidth && height == levelHeight;
		unsigned format = GetSRGB() ? GetSRGBFormat(format_) : format_;

		if(!IsCompressed())
		{
			if(wholeLevel)
				glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),level,format,width,height,0,YumeGLTexture2D::GetExternalFormat(format_),
				YumeGLTexture2D::GetDataType(format_),data);
			else
				glTexSubImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),level,x,y,width,height,YumeGLTexture2D::GetExternalFormat(format_),
				YumeGLTexture2D::GetDataType(format_),data);
		}
		else
		{
			if(wholeLevel)
				glCompressedTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),level,format,width,height,0,
				GetDataSize(width,height),data);
			else
				glCompressedTexSubImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),level,x,y,width,height,format,
				GetDataSize(width,height),data);
		}

		gYume->pRHI->SetTexture(0,0);
		return true;
	}

	bool YumeGLTextureCube::SetData(CubeMapFace face,YumeFile& source)
	{
		SharedPtr<YumeImage> image(new YumeImage);
		if(!image->Load(source))
			return false;

		return SetData(face,image);
	}

	bool YumeGLTextureCube::SetData(CubeMapFace face,SharedPtr<YumeImage> image,bool useAlpha)
	{
		if(!image)
		{
			YUMELOG_ERROR("Null image, can not load texture");
			return false;
		}

		unsigned memoryUse = 0;

		int quality = QUALITY_HIGH;
		YumeRenderer* renderer = gYume->pRenderer.get();
		if(renderer)
			quality = renderer->GetTextureQuality();

		if(!image->IsCompressed())
		{
			// Convert unsuitable formats to RGBA
			unsigned components = image->GetComponents();
			if(YumeGLRenderer::GetGL3Support() && ((components == 1 && !useAlpha) || components == 2))
			{
				image = image->ConvertToRGBA();
				if(!image)
					return false;
				components = image->GetComponents();
			}

			unsigned char* levelData = image->GetData();
			int levelWidth = image->GetWidth();
			int levelHeight = image->GetHeight();
			unsigned format = 0;

			if(levelWidth != levelHeight)
			{
				YUMELOG_ERROR("Cube texture width not equal to height");
				return false;
			}

			// Discard unnecessary mip levels
			for(unsigned i = 0; i < mipsToSkip_[quality]; ++i)
			{
				image = image->GetNextLevel();
				levelData = image->GetData();
				levelWidth = image->GetWidth();
				levelHeight = image->GetHeight();
			}

			switch(components)
			{
			case 1:
				format = useAlpha ? YumeGLRenderer::GetAlphaFormat() : YumeGLRenderer::GetLuminanceFormat();
				break;

			case 2:
				format = YumeGLRenderer::GetLuminanceAlphaFormat();
				break;

			case 3:
				format = YumeGLRenderer::GetRGBFormat();
				break;

			case 4:
				format = YumeGLRenderer::GetRGBAFormat();
				break;

			default:
				assert(false);  // Should not reach here
				break;
			}

			// Create the texture when face 0 is being loaded, check that rest of the faces are same size & format
			if(!face)
			{
				// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
				if(IsCompressed() && requestedLevels_ > 1)
					requestedLevels_ = 0;
				SetSize(levelWidth,format);
			}
			else
			{
				if(!object_)
				{
					YUMELOG_ERROR("Cube texture face 0 must be loaded first");
					return false;
				}
				if(levelWidth != width_ || format != format_)
				{
					YUMELOG_ERROR("Cube texture face does not match size or format of face 0");
					return false;
				}
			}

			for(unsigned i = 0; i < levels_; ++i)
			{
				SetData(face,i,0,0,levelWidth,levelHeight,levelData);
				memoryUse += levelWidth * levelHeight * components;

				if(i < levels_ - 1)
				{
					image = image->GetNextLevel();
					levelData = image->GetData();
					levelWidth = image->GetWidth();
					levelHeight = image->GetHeight();
				}
			}
		}
		else
		{
			int width = image->GetWidth();
			int height = image->GetHeight();
			unsigned levels = image->GetNumCompressedLevels();
			unsigned format = gYume->pRHI->GetFormat(image->GetCompressedFormat());
			bool needDecompress = false;

			if(width != height)
			{
				YUMELOG_ERROR("Cube texture width not equal to height");
				return false;
			}

			if(!format)
			{
				format = YumeGLRenderer::GetRGBAFormat();
				needDecompress = true;
			}

			unsigned mipsToSkip = mipsToSkip_[quality];
			if(mipsToSkip >= levels)
				mipsToSkip = levels - 1;
			while(mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4))
				--mipsToSkip;
			width /= (1 << mipsToSkip);
			height /= (1 << mipsToSkip);

			// Create the texture when face 0 is being loaded, assume rest of the faces are same size & format
			if(!face)
			{
				SetNumLevels(std::max((levels - mipsToSkip),1U));
				SetSize(width,format);
			}
			else
			{
				if(!object_)
				{
					YUMELOG_ERROR("Cube texture face 0 must be loaded first");
					return false;
				}
				if(width != width_ || format != format_)
				{
					YUMELOG_ERROR("Cube texture face does not match size or format of face 0");
					return false;
				}
			}

			for(unsigned i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
			{
				CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
				if(!needDecompress)
				{
					SetData(face,i,0,0,level.width_,level.height_,level.data_);
					memoryUse += level.rows_ * level.rowSize_;
				}
				else
				{
					unsigned char* rgbaData = new unsigned char[level.width_ * level.height_ * 4];
					level.Decompress(rgbaData);
					SetData(face,i,0,0,level.width_,level.height_,rgbaData);
					memoryUse += level.width_ * level.height_ * 4;
					delete[] rgbaData;
				}
			}
		}

		faceMemoryUse_[face] = memoryUse;
		unsigned totalMemoryUse = sizeof(YumeGLTextureCube);
		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
			totalMemoryUse += faceMemoryUse_[i];
		SetMemoryUsage(totalMemoryUse);
		return true;
	}

	bool YumeGLTextureCube::GetData(CubeMapFace face,unsigned level,void* dest) const
	{
		if(!object_)
		{
			YUMELOG_ERROR("No texture created, can not get data");
			return false;
		}

		if(!dest)
		{
			YUMELOG_ERROR("Null destination for getting data");
			return false;
		}

		if(level >= levels_)
		{
			YUMELOG_ERROR("Illegal mip level for getting data");
			return false;
		}

		static_cast<YumeGLRenderer*>(gYume->pRHI)->SetTextureForUpdate(const_cast<YumeGLTextureCube*>(this));

		if(!IsCompressed())
			glGetTexImage((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),level,YumeGLTexture2D::GetExternalFormat(format_),YumeGLTexture2D::GetDataType(format_),dest);
		else
			glGetCompressedTexImage((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face),level,dest);

		gYume->pRHI->SetTexture(0,0);
		return true;
	}

	bool YumeGLTextureCube::Create()
	{
		Release();

		if(!gYume->pRHI || !width_ || !height_)
			return false;

		if(gYume->pRHI->IsDeviceLost())
		{
			YUMELOG_WARN("Texture creation while device is lost");
			return true;
		}

		glGenTextures(1,&object_);

		// Ensure that our texture is bound to OpenGL texture unit 0
		static_cast<YumeGLRenderer*>(gYume->pRHI)->SetTextureForUpdate(this);

		// If not compressed, create the initial level 0 texture with null data
		unsigned format = GetSRGB() ? GetSRGBFormat(format_) : format_;
		unsigned externalFormat = YumeGLTexture2D::GetExternalFormat(format_);
		unsigned dataType = YumeGLTexture2D::GetDataType(format_);

		bool success = true;
		if(!IsCompressed())
		{
			glGetError();
			for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,format,width_,height_,0,externalFormat,dataType,0);
				if(glGetError())
					success = false;
			}
		}
		if(!success)
			YUMELOG_ERROR("Failed to create texture");

		// Set mipmapping
		levels_ = requestedLevels_;
		if(!levels_)
		{
			unsigned maxSize = (unsigned)Max(width_,height_);
			while(maxSize)
			{
				maxSize >>= 1;
				++levels_;
			}
		}

#ifndef GL_ES_VERSION_2_0
		glTexParameteri(target_,GL_TEXTURE_BASE_LEVEL,0);
		glTexParameteri(target_,GL_TEXTURE_MAX_LEVEL,levels_ - 1);
#endif

		// Set initial parameters, then unbind the texture
		UpdateParameters();
		gYume->pRHI->SetTexture(0,0);

		return success;
	}

	void YumeGLTextureCube::UpdateParameters()
	{
		if(!object_ || !gYume->pRHI)
			return;

		// Wrapping
		glTexParameteri(target_,GL_TEXTURE_WRAP_S,GetWrapMode(addressMode_[COORD_U]));
		glTexParameteri(target_,GL_TEXTURE_WRAP_T,GetWrapMode(addressMode_[COORD_V]));
		glTexParameteri(target_,GL_TEXTURE_WRAP_R,GetWrapMode(addressMode_[COORD_W]));

		TextureFilterMode filterMode = filterMode_;
		if(filterMode == FILTER_DEFAULT)
			filterMode = gYume->pRHI->GetDefaultTextureFilterMode();

		// Filtering
		switch(filterMode)
		{
		case FILTER_NEAREST:
			glTexParameteri(target_,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(target_,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			break;

		case FILTER_BILINEAR:
			if(levels_ < 2)
				glTexParameteri(target_,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			else
				glTexParameteri(target_,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(target_,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;

		case FILTER_ANISOTROPIC:
		case FILTER_TRILINEAR:
			if(levels_ < 2)
				glTexParameteri(target_,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			else
				glTexParameteri(target_,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(target_,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;

		default:
			break;
		}

#ifndef GL_ES_VERSION_2_0
		// Anisotropy
		if(gYume->pRHI->GetAnisotropySupport())
		{
			glTexParameterf(target_,GL_TEXTURE_MAX_ANISOTROPY_EXT,
				filterMode == FILTER_ANISOTROPIC ? (float)gYume->pRHI->GetTextureAnisotropy() : 1.0f);
		}

		// Shadow compare
		if(shadowCompare_)
		{
			glTexParameteri(target_,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
			glTexParameteri(target_,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
		}
		else
			glTexParameteri(target_,GL_TEXTURE_COMPARE_MODE,GL_NONE);

		glTexParameterfv(target_,GL_TEXTURE_BORDER_COLOR,borderColor_.Data());
#endif

		parametersDirty_ = false;
	}

	void YumeGLTextureCube::CheckTextureBudget(YumeHash type)
	{

	}

	void YumeGLTextureCube::HandleRenderTargetUpdate()
	{
		YumeRenderer* renderer = gYume->pRenderer.get();

		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
		{
			if(renderSurfaces_[i] && (renderSurfaces_[i]->GetUpdateMode() == SURFACE_UPDATEALWAYS || renderSurfaces_[i]->IsUpdateQueued()))
			{
				if(renderer)
					renderer->QueueRenderable(renderSurfaces_[i].get());
				renderSurfaces_[i]->ResetUpdateQueued();
			}
		}
	}

	bool YumeGLTextureCube::IsCompressed() const
	{
		return format_ == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || format_ == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
			format_ == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT || format_ == GL_ETC1_RGB8_OES ||
			format_ == COMPRESSED_RGB_PVRTC_4BPPV1_IMG || format_ == COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ||
			format_ == COMPRESSED_RGB_PVRTC_2BPPV1_IMG || format_ == COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	}

	unsigned YumeGLTextureCube::GetDataSize(int width,int height) const
	{
		if(IsCompressed())
		{
			if(format_ == COMPRESSED_RGB_PVRTC_4BPPV1_IMG || format_ == COMPRESSED_RGBA_PVRTC_4BPPV1_IMG)
				return (unsigned)((Max(width,8) * Max(height,8) * 4 + 7) >> 3);
			else if(format_ == COMPRESSED_RGB_PVRTC_2BPPV1_IMG || format_ == COMPRESSED_RGBA_PVRTC_2BPPV1_IMG)
				return (unsigned)((Max(width,16) * Max(height,8) * 2 + 7) >> 3);
			else
				return GetRowDataSize(width) * ((height + 3) >> 2);
		}
		else
			return GetRowDataSize(width) * height;
	}

	unsigned YumeGLTextureCube::GetRowDataSize(int width) const
	{
		switch(format_)
		{
		case GL_ALPHA:
		case GL_LUMINANCE:
			return (unsigned)width;

		case GL_LUMINANCE_ALPHA:
			return (unsigned)(width * 2);

		case GL_RGB:
			return (unsigned)(width * 3);

		case GL_RGBA:
#ifndef GL_ES_VERSION_2_0
		case GL_DEPTH24_STENCIL8_EXT:
		case GL_RG16:
		case GL_RG16F:
		case GL_R32F:
#endif
			return (unsigned)(width * 4);

#ifndef GL_ES_VERSION_2_0
		case GL_R8:
			return (unsigned)width;

		case GL_RG8:
		case GL_R16F:
			return (unsigned)(width * 2);

		case GL_RGBA16:
		case GL_RGBA16F_ARB:
			return (unsigned)(width * 8);

		case GL_RGBA32F_ARB:
			return (unsigned)(width * 16);
#endif

		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return (unsigned)(((width + 3) >> 2) * 8);

		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return (unsigned)(((width + 3) >> 2) * 16);

		case GL_ETC1_RGB8_OES:
			return (unsigned)(((width + 3) >> 2) * 8);

		case COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			return (unsigned)((width * 4 + 7) >> 3);

		case COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
			return (unsigned)((width * 2 + 7) >> 3);

		default:
			return 0;
		}
	}

}
