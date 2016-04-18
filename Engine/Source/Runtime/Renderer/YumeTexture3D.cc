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
#include "YumeTexture3D.h"
#include "Core/YumeFile.h"
#include "YumeImage.h"
#include "YumeResourceManager.h"
#include "YumeXmlFile.h"
#include "Engine/YumeEngine.h"
#include "Core/YumeXmlParser.h"

#include "Logging/logging.h"
#include "pugixml/src/pugixml.hpp"

namespace YumeEngine
{
	YumeHash YumeTexture3D::type_ = ("Texture3D");

	YumeTexture3D::YumeTexture3D()
	{
	}

	YumeTexture3D::~YumeTexture3D()
	{

	}

	bool YumeTexture3D::BeginLoad(YumeFile& source)
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;

		YumeString texPath,texName,texExt;
		SplitPath(GetName(),texPath,texName,texExt);

		rm_->ResetDependencies(this);

		SharedPtr<YumeFile> file = rm_->GetFile(GetName());
		loadImage_ = SharedPtr<YumeImage>(YumeAPINew YumeImage);
		if(!loadImage_->Load(*(file)))
		{
			loadParameters_.Reset();
			loadImage_.Reset();
			return false;
		}
		return true;
	}

	bool YumeTexture3D::EndLoad()
	{
		// If over the texture budget, see if materials can be freed to allow textures to be freed
		CheckTextureBudget(GetType());

		bool success = SetData(loadImage_);

		loadImage_.Reset();

		return success;
	}
}
