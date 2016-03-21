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

		loadParameters_ = SharedPtr<YumeXmlFile>(YumeAPINew YumeXmlFile());
		if(!loadParameters_->Load(source))
		{
			loadParameters_.Reset();
			return false;
		}

		YumeString xmlData = loadParameters_->GetXml();
		pugi::xml_document xmlDoc;
		xmlDoc.load(xmlData.c_str());

		XmlNode textureElem = xmlDoc.child("texture3d");
		XmlNode volumeElem = textureElem.child("volume");
		XmlNode colorlutElem = textureElem.child("colorlut");

		if(!volumeElem.empty())
		{
			YumeString name = volumeElem.attribute("name").as_string();

			YumeString volumeTexPath,volumeTexName,volumeTexExt;
			SplitPath(name,volumeTexPath,volumeTexName,volumeTexExt);
			// If path is empty, add the XML file path
			if(volumeTexPath.empty())
				name = texPath + name;

			loadImage_ = rm_->GetTempResource<YumeImage>(name);
			// Precalculate mip levels if async loading
			if(loadImage_ && GetAsyncLoadState() == ASYNC_LOADING)
				loadImage_->PrecalculateLevels();
			rm_->StoreResourceDependency(this,name);
			return true;
		}
		else if(!colorlutElem.empty())
		{
			YumeString name = colorlutElem.attribute("name").as_string();

			YumeString colorlutTexPath,colorlutTexName,colorlutTexExt;
			SplitPath(name,colorlutTexPath,colorlutTexName,colorlutTexExt);
			// If path is empty, add the XML file path
			if(colorlutTexPath.empty())
				name = texPath + name;

			SharedPtr<YumeFile> file = rm_->GetFile(name);
			loadImage_ = SharedPtr<YumeImage>(YumeAPINew YumeImage);
			if(!loadImage_->LoadColorLUT(*(file)))
			{
				loadParameters_.Reset();
				loadImage_.Reset();
				return false;
			}
			// Precalculate mip levels if async loading
			if(loadImage_ && GetAsyncLoadState() == ASYNC_LOADING)
				loadImage_->PrecalculateLevels();
			rm_->StoreResourceDependency(this,name);
			return true;
		}

		YUMELOG_ERROR("Texture3D XML data for " << GetName().c_str() << " did not contain either volume or colorlut element");
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
