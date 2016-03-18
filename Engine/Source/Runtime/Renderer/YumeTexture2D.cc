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
#include "YumeTexture2D.h"

#include "YumeRenderable.h"
#include "YumeImage.h"

#include "Core/YumeDefaults.h"
#include "Engine/YumeEngine.h"
#include "Renderer/YumeResourceManager.h"

#include "Core/YumeXmlFile.h"
namespace YumeEngine
{
	YumeHash YumeTexture2D::type_ = ("Texture2D");

	YumeTexture2D::YumeTexture2D()
	{
	}

	YumeTexture2D::~YumeTexture2D()
	{

	}

	bool YumeTexture2D::BeginLoad(YumeFile& source)
	{

		// Load the image data for EndLoad()
		loadImage_ = SharedPtr<YumeImage>(YumeAPINew YumeImage);
		if(!loadImage_->Load(source))
		{
			loadImage_.Reset();
			return false;
		}

		// Precalculate mip levels if async loading
		if(GetAsyncLoadState() == ASYNC_LOADING)
			loadImage_->PrecalculateLevels();

		// Load the optional parameters file
		YumeResourceManager* rm_ = gYume->pResourceManager;
		YumeString xmlName;
		YumeString path,file,extension;
		SplitPath(GetName(),path,file,extension);
		xmlName =  path + file + ".xml";
		loadParameters_ = rm_->GetTempResource<YumeXmlFile>(xmlName);

		return true;
	}

	bool YumeTexture2D::EndLoad()
	{
		// If over the texture budget, see if materials can be freed to allow textures to be freed
		CheckTextureBudget(GetType());

		if(loadParameters_)
			SetParameters(loadParameters_);

		bool success = SetData(loadImage_);

		loadImage_.Reset();

		return success;
	}
}
