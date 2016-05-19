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
#include "YumeTexture.h"
#include "YumeRHI.h"
#include "Engine/YumeEngine.h"
#include "Core/YumeXmlFile.h"

#include "Core/YumeDefaults.h"

namespace YumeEngine
{

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

	YumeHash YumeTexture::textureType_ = "Texture";

	YumeTexture::YumeTexture()
		: shaderResourceView_(0),
		unorderedAccessView_(0),
		sampler_(0),
		usage_(TEXTURE_STATIC),
		levels_(0),
		requestedLevels_(0),
		width_(0),
		height_(0),
		depth_(0),
		shadowCompare_(false),
		filterMode_(FILTER_DEFAULT),
		sRGB_(false),
		parametersDirty_(true),
		arraySize_(1),
		arraySlice_(0)
	{
		for(int i = 0; i < MAX_COORDS; ++i)
			addressMode_[i] = ADDRESS_WRAP;
		for(int i = 0; i < MAX_TEXTURE_QUALITY_LEVELS; ++i)
			mipsToSkip_[i] = (unsigned)(MAX_TEXTURE_QUALITY_LEVELS - 1 - i);
	}

	YumeTexture::~YumeTexture()
	{
	}


	void YumeTexture::SetNumLevels(unsigned levels)
	{
		if(usage_ > TEXTURE_RENDERTARGET)
			requestedLevels_ = 1;
		else
			requestedLevels_ = levels;
	}

	void YumeTexture::SetFilterMode(TextureFilterMode mode)
	{
		filterMode_ = mode;
		parametersDirty_ = true;
	}

	void YumeTexture::SetAddressMode(TextureCoordinate coord,TextureAddressMode mode)
	{
		addressMode_[coord] = mode;
		parametersDirty_ = true;
	}

	void YumeTexture::SetShadowCompare(bool enable)
	{
		shadowCompare_ = enable;
		parametersDirty_ = true;
	}

	void YumeTexture::SetBorderColor(const YumeColor& color)
	{
		borderColor_ = color;
		parametersDirty_ = true;
	}

	void YumeTexture::SetSRGB(bool enable)
	{
		if(gYume->pRHI)
			enable &= gYume->pRHI->GetSRGBSupport();

		sRGB_ = enable;
	}

	void YumeTexture::SetBackupTexture(YumeTexture* texture)
	{
		backupTexture_ = SharedPtr<YumeTexture>(texture);
	}

	void YumeTexture::SetMipsToSkip(int quality,int toSkip)
	{
		if(quality >= QUALITY_LOW && quality < MAX_TEXTURE_QUALITY_LEVELS)
		{
			mipsToSkip_[quality] = (unsigned)toSkip;

			// Make sure a higher quality level does not actually skip more mips
			for(int i = 1; i < MAX_TEXTURE_QUALITY_LEVELS; ++i)
			{
				if(mipsToSkip_[i] > mipsToSkip_[i - 1])
					mipsToSkip_[i] = mipsToSkip_[i - 1];
			}
		}
	}

	int YumeTexture::GetMipsToSkip(int quality) const
	{
		return (quality >= QUALITY_LOW && quality < MAX_TEXTURE_QUALITY_LEVELS) ? mipsToSkip_[quality] : 0;
	}

	int YumeTexture::GetLevelWidth(unsigned level) const
	{
		if(level > levels_)
			return 0;
		return std::max(width_ >> level,1);
	}

	int YumeTexture::GetLevelHeight(unsigned level) const
	{
		if(level > levels_)
			return 0;
		return std::max(height_ >> level,1);
	}

	int YumeTexture::GetLevelDepth(unsigned level) const
	{
		if(level > levels_)
			return 0;
		return std::max(depth_ >> level,1);
	}


	unsigned YumeTexture::GetDataSize(int width,int height,int depth) const
	{
		return depth * GetDataSize(width,height);
	}


	unsigned YumeTexture::GetComponents() const
	{
		if(!width_ || IsCompressed())
			return 0;
		else
			return GetRowDataSize(width_) / width_;
	}

	void YumeTexture::SetParameters(YumeXmlFile* file)
	{
		YumeString xmlData = file->GetXml();
		pugi::xml_document doc;
		doc.load(xmlData.c_str());
		XmlNode root = doc.root();
		SetParameters(root);
	}

	void YumeTexture::SetParameters(const XmlNode& element)
	{
		XmlNode texture = element.child("texture");
		for(XmlNode child = texture.first_child(); child; child = child.next_sibling())
		{
			YumeString name = child.name();

			if(name == "address")
			{
				YumeString coord = child.attribute("coord").as_string();
				if(coord.length() >= 1)
				{
					TextureCoordinate coordIndex = (TextureCoordinate)(coord[0] - 'u');
					YumeString mode = child.attribute("mode").as_string();
					SetAddressMode(coordIndex,(TextureAddressMode)GetStringListIndex(mode.c_str(),addressModeNames,ADDRESS_WRAP));
				}
			}

			if(name == "border")
				SetBorderColor(ToColor(child.attribute("color").as_string()));

			if(name == "filter")
			{
				YumeString mode = child.attribute("mode").as_string();
				SetFilterMode((TextureFilterMode)GetStringListIndex(mode.c_str(),filterModeNames,FILTER_DEFAULT));
			}

			if(name == "mipmap")
				SetNumLevels(child.attribute("enable").as_bool() ? 0 : 1);

			if(name == "quality")
			{
				if(child.attribute("low").empty())
					SetMipsToSkip(QUALITY_LOW,child.attribute("low").as_int());
				if(child.attribute("med").empty())
					SetMipsToSkip(QUALITY_MEDIUM,child.attribute("med").as_int());
				if(child.attribute("medium").empty())
					SetMipsToSkip(QUALITY_MEDIUM,child.attribute("medium").as_int());
				if(child.attribute("high").empty())
					SetMipsToSkip(QUALITY_HIGH,child.attribute("high").as_int());
			}

			if(name == "srgb")
				SetSRGB(child.attribute("enable").as_bool());
		}
	}
	void YumeTexture::SetParametersDirty()
	{
		parametersDirty_ = true;
	}


	unsigned YumeTexture::CheckMaxLevels(int width,int height,unsigned requestedLevels)
	{
		unsigned maxLevels = 1;
		while(width > 1 && height > 1)
		{
			++maxLevels;
			width >>= 1;
			height >>= 1;
		}

		if(!requestedLevels || maxLevels < requestedLevels)
			return maxLevels;
		else
			return requestedLevels;
	}

	unsigned YumeTexture::CheckMaxLevels(int width,int height,int depth,unsigned requestedLevels)
	{
		unsigned maxLevels = 1;
		while(width > 1 && height > 1 && depth > 1)
		{
			++maxLevels;
			width >>= 1;
			height >>= 1;
			depth >>= 1;
		}

		if(!requestedLevels || maxLevels < requestedLevels)
			return maxLevels;
		else
			return requestedLevels;
	}

}
