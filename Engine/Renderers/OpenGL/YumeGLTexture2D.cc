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

#include "YumeGLRenderer.h"
#include "Renderer/YumeRHI.h"

#include "YumeGLRendererImpl.h"

#include "Renderer/YumeImage.h"
#include "Renderer/YumeRenderer.h"
#include "YumeGLRenderable.h"

#include "Engine/YumeEngine.h"
#include "Renderer/YumeResourceManager.h"

#include "Logging/logging.h"


namespace YumeEngine
{

	static GLenum glWrapModes[] =
	{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
#ifndef GL_ES_VERSION_2_0
		GL_CLAMP
#else
		GL_CLAMP_TO_EDGE
#endif
	};

#ifndef GL_ES_VERSION_2_0
	static GLenum gl3WrapModes[] =
	{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER
	};
#endif

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
#ifndef GL_ES_VERSION_2_0
		return YumeGLRenderer::GetGL3Support() ? gl3WrapModes[mode] : glWrapModes[mode];
#else
		return glWrapModes[mode];
#endif
	}
	YumeGLTexture2D::YumeGLTexture2D(YumeRHI* rhi)
		: YumeGLResource((rhi))
	{
		target_ = GL_TEXTURE_2D;

	}

	YumeGLTexture2D::~YumeGLTexture2D()
	{
		Release();
	}

	void YumeGLTexture2D::OnDeviceLost()
	{
		YumeGLResource::OnDeviceLost();

		if(renderSurface_)
			boost::static_pointer_cast<YumeGLRenderable>(renderSurface_)->OnDeviceLost();
	}

	void YumeGLTexture2D::OnDeviceReset()
	{
		if(!object_ || dataPending_)
		{
			// If has a resource file, reload through the resource cache. Otherwise just recreate.
			YumeResourceManager* rm_ = YumeEngine3D::Get()->GetResourceManager();
			//if(rm_ ->Exists(GetName()))
				//dataLost_ = !rm_ ->ReloadResource(this);

			if(!object_)
			{
				Create();
				dataLost_ = true;
			}
		}

		dataPending_ = false;
	}

	void YumeGLTexture2D::Release()
	{
		if(object_)
		{
			if(!rhi_)
				return;

			if(!rhi_->IsDeviceLost())
			{
				for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
				{
					if(rhi_->GetTexture(i) == this)
						rhi_->SetTexture(i,0);
				}

				glDeleteTextures(1,(GLuint*)&object_);
			}

			if(renderSurface_)
				renderSurface_->Release();

			object_ = 0;
		}
		else
		{
			if(renderSurface_)
				renderSurface_->Release();
		}
	}


	bool YumeGLTexture2D::SetSize(int width,int height,unsigned format,TextureUsage usage)
	{
		if(width <= 0 || height <= 0)
		{
			YUMELOG_ERROR("Zero or negative texture dimensions");
			return false;
		}

		// Delete the old rendersurface if any
		renderSurface_.reset();
		usage_ = usage;

		if(usage_ == TEXTURE_RENDERTARGET || usage_ == TEXTURE_DEPTHSTENCIL)
		{
			renderSurface_ = SharedPtr<YumeRenderable>(new YumeGLRenderable(this));

			// Clamp mode addressing by default, nearest filtering, and mipmaps disabled
			addressMode_[COORD_U] = ADDRESS_CLAMP;
			addressMode_[COORD_V] = ADDRESS_CLAMP;
			filterMode_ = FILTER_NEAREST;
			requestedLevels_ = 1;
		}
		else if(usage_ == TEXTURE_DYNAMIC)
			requestedLevels_ = 1;

		//if(usage_ == TEXTURE_RENDERTARGET)
		//	SubscribeToEvent(E_RENDERSURFACEUPDATE,URHO3D_HANDLER(Texture2D,HandleRenderSurfaceUpdate));
		//else
		//	UnsubscribeFromEvent(E_RENDERSURFACEUPDATE);

		width_ = width;
		height_ = height;
		format_ = format;

		return Create();
	}

	unsigned YumeGLTexture2D::GetExternalFormat(unsigned format)
	{
#ifndef GL_ES_VERSION_2_0
		if(format == GL_DEPTH_COMPONENT16 || format == GL_DEPTH_COMPONENT24 || format == GL_DEPTH_COMPONENT32)
			return GL_DEPTH_COMPONENT;
		else if(format == GL_DEPTH24_STENCIL8_EXT)
			return GL_DEPTH_STENCIL_EXT;
		else if(format == GL_SLUMINANCE_EXT)
			return GL_LUMINANCE;
		else if(format == GL_SLUMINANCE_ALPHA_EXT)
			return GL_LUMINANCE_ALPHA;
		else if(format == GL_R8 || format == GL_R16F || format == GL_R32F)
			return GL_RED;
		else if(format == GL_RG8 || format == GL_RG16 || format == GL_RG16F || format == GL_RG32F)
			return GL_RG;
		else if(format == GL_RGBA16 || format == GL_RGBA16F_ARB || format == GL_RGBA32F_ARB || format == GL_SRGB_ALPHA_EXT)
			return GL_RGBA;
		else if(format == GL_SRGB_EXT)
			return GL_RGB;
		else
			return format;
#else
		return format;
#endif
	}

	bool YumeGLTexture2D::SetData(unsigned level,int x,int y,int width,int height,const void* data)
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


		if(rhi_->IsDeviceLost())
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

		static_cast<YumeGLRenderer*>(rhi_)->SetTextureForUpdate(this);

		bool wholeLevel = x == 0 && y == 0 && width == levelWidth && height == levelHeight;
		unsigned format = GetSRGB() ? GetSRGBFormat(format_) : format_;

		if(!IsCompressed())
		{
			if(wholeLevel)
				glTexImage2D(target_,level,format,width,height,0,GetExternalFormat(format_),GetDataType(format_),data);
			else
				glTexSubImage2D(target_,level,x,y,width,height,GetExternalFormat(format_),GetDataType(format_),data);
		}
		else
		{
			if(wholeLevel)
				glCompressedTexImage2D(target_,level,format,width,height,0,GetDataSize(width,height),data);
			else
				glCompressedTexSubImage2D(target_,level,x,y,width,height,format,GetDataSize(width,height),data);
		}

		rhi_->SetTexture(0,0);
		return true;
	}

	bool YumeGLTexture2D::SetData(SharedPtr<YumeImage> image,bool useAlpha)
	{
		if(!image)
		{
			YUMELOG_ERROR("Null image, can not set data");
			return false;
		}

		unsigned memoryUse = sizeof(YumeGLTexture2D);

		int quality = QUALITY_HIGH;
		YumeRenderer* renderer = 0;
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

			// If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
			if(IsCompressed() && requestedLevels_ > 1)
				requestedLevels_ = 0;
			SetSize(levelWidth,levelHeight,format);
			if(!object_)
				return false;

			for(unsigned i = 0; i < levels_; ++i)
			{
				SetData(i,0,0,levelWidth,levelHeight,levelData);
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
			unsigned format = rhi_->GetFormat(image->GetCompressedFormat());
			bool needDecompress = false;

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

			SetNumLevels((unsigned)Max((int)(levels - mipsToSkip),1));
			SetSize(width,height,format);

			for(unsigned i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
			{
				CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
				if(!needDecompress)
				{
					SetData(i,0,0,level.width_,level.height_,level.data_);
					memoryUse += level.rows_ * level.rowSize_;
				}
				else
				{
					unsigned char* rgbaData = new unsigned char[level.width_ * level.height_ * 4];
					level.Decompress(rgbaData);
					SetData(i,0,0,level.width_,level.height_,rgbaData);
					memoryUse += level.width_ * level.height_ * 4;
					delete[] rgbaData;
				}
			}
		}

		SetMemoryUsage(memoryUse);
		return true;
	}

	bool YumeGLTexture2D::GetData(unsigned level,void* dest) const
	{
#ifndef GL_ES_VERSION_2_0
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
		if(rhi_->IsDeviceLost())
		{
			YUMELOG_WARN("Getting texture data while device is lost");
			return false;
	}

		static_cast<YumeGLRenderer*>(rhi_)->SetTextureForUpdate(const_cast<YumeGLTexture2D*>(this));

		if(!IsCompressed())
			glGetTexImage(target_,level,GetExternalFormat(format_),GetDataType(format_),dest);
		else
			glGetCompressedTexImage(target_,level,dest);

		rhi_->SetTexture(0,0);
		return true;
#else
		YUMELOG_ERROR("Getting texture data not supported");
		return false;
#endif
}

	bool YumeGLTexture2D::Create()
	{
		Release();

		if(!rhi_ || !width_ || !height_)
			return false;

		if(rhi_->IsDeviceLost())
		{
			YUMELOG_WARN("Texture creation while device is lost");
			return true;
	}

		unsigned format = GetSRGB() ? GetSRGBFormat(format_) : format_;
		unsigned externalFormat = GetExternalFormat(format_);
		unsigned dataType = GetDataType(format_);

		// Create a renderbuffer instead of a texture if depth texture is not properly supported, or if this will be a packed
		// depth stencil texture
#ifndef GL_ES_VERSION_2_0
		if(format == YumeGLRenderer::GetDepthStencilFormat())
#else
		if(format == GL_DEPTH_COMPONENT16 || format == GL_DEPTH_COMPONENT24_OES || format == GL_DEPTH24_STENCIL8_OES ||
			(format == GL_DEPTH_COMPONENT && !graphics_->GetShadowMapFormat()))
#endif
		{
			if(renderSurface_)
			{
				boost::static_pointer_cast<YumeGLRenderable>(renderSurface_)->CreateRenderBuffer(width_,height_,format);
				return true;
			}
			else
				return false;
		}

		glGenTextures(1,(GLuint*)&object_);

		// Ensure that our texture is bound to OpenGL texture unit 0
		static_cast<YumeGLRenderer*>(rhi_)->SetTextureForUpdate(this);

		// If not compressed, create the initial level 0 texture with null data
		bool success = true;

		if(!IsCompressed())
		{
			glGetError();
			glTexImage2D(target_,0,format,width_,height_,0,externalFormat,dataType,0);
			if(glGetError())
			{
				YUMELOG_ERROR("Failed to create texture");
				success = false;
			}
		}

		// Set mipmapping
		levels_ = requestedLevels_;
		if(!levels_)
		{
			unsigned maxSize = Max((int)width_,(int)height_);
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
		rhi_->SetTexture(0,0);

		return success;
	}

	void YumeGLTexture2D::HandleRenderSurfaceUpdate(YumeHash eventType,VariantMap& eventData)
	{
		if(renderSurface_ && (renderSurface_->GetUpdateMode() == SURFACE_UPDATEALWAYS || renderSurface_->IsUpdateQueued()))
		{
			YumeRenderer* renderer = 0;
			if(renderer)
				renderer->QueueRenderable(renderSurface_.get());
			renderSurface_->ResetUpdateQueued();
		}
	}

	unsigned YumeGLTexture2D::GetDataSize(int width,int height) const
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


	unsigned YumeGLTexture2D::GetRowDataSize(int width) const
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
	void YumeGLTexture2D::UpdateParameters()
	{
		if(!object_ || !rhi_)
			return;

		// Wrapping
		glTexParameteri(target_,GL_TEXTURE_WRAP_S,GetWrapMode(addressMode_[COORD_U]));
		glTexParameteri(target_,GL_TEXTURE_WRAP_T,GetWrapMode(addressMode_[COORD_V]));
#ifndef GL_ES_VERSION_2_0
		glTexParameteri(target_,GL_TEXTURE_WRAP_R,GetWrapMode(addressMode_[COORD_W]));
#endif

		TextureFilterMode filterMode = filterMode_;
		if(filterMode == FILTER_DEFAULT)
			filterMode = rhi_->GetDefaultTextureFilterMode();

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
		if(static_cast<YumeGLRenderer*>(rhi_)->GetAnisotropySupport())
		{
			glTexParameterf(target_,GL_TEXTURE_MAX_ANISOTROPY_EXT,
				filterMode == FILTER_ANISOTROPIC ? (float)rhi_->GetTextureAnisotropy() : 1.0f);
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


	unsigned YumeGLTexture2D::GetDataType(unsigned format)
	{
#ifndef GL_ES_VERSION_2_0
		if(format == GL_DEPTH24_STENCIL8_EXT)
			return GL_UNSIGNED_INT_24_8_EXT;
		else if(format == GL_RG16 || format == GL_RGBA16)
			return GL_UNSIGNED_SHORT;
		else if(format == GL_RGBA32F_ARB || format == GL_RG32F || format == GL_R32F)
			return GL_FLOAT;
		else if(format == GL_RGBA16F_ARB || format == GL_RG16F || format == GL_R16F)
			return GL_HALF_FLOAT_ARB;
		else
			return GL_UNSIGNED_BYTE;
#else
		if(format == GL_DEPTH_COMPONENT || format == GL_DEPTH_COMPONENT24_OES)
			return GL_UNSIGNED_INT;
		else if(format == GL_DEPTH_COMPONENT16)
			return GL_UNSIGNED_SHORT;
		else
			return GL_UNSIGNED_BYTE;
#endif
	}

	void YumeGLTexture2D::CheckTextureBudget(YumeHash type)
	{

	}

	bool YumeGLTexture2D::IsCompressed() const
	{
		return format_ == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || format_ == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
			format_ == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT || format_ == GL_ETC1_RGB8_OES ||
			format_ == COMPRESSED_RGB_PVRTC_4BPPV1_IMG || format_ == COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ||
			format_ == COMPRESSED_RGB_PVRTC_2BPPV1_IMG || format_ == COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	}

}
