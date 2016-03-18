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
#include "YumeTextureCube.h"

#include "YumeRenderable.h"
#include "YumeImage.h"
#include "YumeResourceManager.h"
#include "YumeXmlFile.h"
#include "Engine/YumeEngine.h"
#include "Core/YumeXmlParser.h"

#include "Logging/logging.h"
#include "pugixml/src/pugixml.hpp"
#include "Core/YumeDefaults.h"

#include "Core/SharedPtr.h"

namespace YumeEngine
{
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

	YumeHash YumeTextureCube::type_ = "TextureCube";

	YumeTextureCube::YumeTextureCube()
		: lockedLevel_(-1)
	{
		// Default to clamp mode addressing
		addressMode_[COORD_U] = ADDRESS_CLAMP;
		addressMode_[COORD_V] = ADDRESS_CLAMP;
		addressMode_[COORD_W] = ADDRESS_CLAMP;

		for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
			faceMemoryUse_[i] = 0;
	}

	YumeTextureCube::~YumeTextureCube()
	{

	}

	bool YumeTextureCube::BeginLoad(YumeFile& source)
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;



		rm_->ResetDependencies(this);

		YumeString texPath,texName,texExt;
		SplitPath(GetName(),texPath,texName,texExt);

		loadParameters_ = SharedPtr<YumeXmlFile>(new YumeXmlFile);
		loadParameters_->Load(source);

		loadImages_.clear();

		YumeString xmlData = loadParameters_->GetXml();
		pugi::xml_document xmlDoc;
		xmlDoc.load(xmlData.c_str());

		XmlNode textureElem = xmlDoc.child("cubemap");
		XmlNode imageElem = textureElem.child("image");
		// Single image and multiple faces with layout
		if(!imageElem.empty())
		{
			YumeString name = imageElem.attribute("name").as_string();
			// If path is empty, add the XML file path
			if(GetPath(name).empty())
				name = texPath + name;

			SharedPtr<YumeImage> image = rm_->GetTempResource<YumeImage>(name);
			if(!image)
				return false;

			int faceWidth,faceHeight;
			loadImages_.resize(MAX_CUBEMAP_FACES);

			if(image->IsCubemap())
			{
				loadImages_[FACE_POSITIVE_X] = image;
				loadImages_[FACE_NEGATIVE_X] = loadImages_[FACE_POSITIVE_X]->GetNextSibling();
				loadImages_[FACE_POSITIVE_Y] = loadImages_[FACE_NEGATIVE_X]->GetNextSibling();
				loadImages_[FACE_NEGATIVE_Y] = loadImages_[FACE_POSITIVE_Y]->GetNextSibling();
				loadImages_[FACE_POSITIVE_Z] = loadImages_[FACE_NEGATIVE_Y]->GetNextSibling();
				loadImages_[FACE_NEGATIVE_Z] = loadImages_[FACE_POSITIVE_Z]->GetNextSibling();
			}
			else
			{

				CubeMapLayout layout =
					(CubeMapLayout)GetStringListIndex(imageElem.attribute("layout").as_string(),cubeMapLayoutNames,CML_HORIZONTAL);

				switch(layout)
				{
				case CML_HORIZONTAL:
					faceWidth = image->GetWidth() / MAX_CUBEMAP_FACES;
					faceHeight = image->GetHeight();
					loadImages_[FACE_POSITIVE_Z] = GetTileImage(image,0,0,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_X] = GetTileImage(image,1,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Z] = GetTileImage(image,2,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_X] = GetTileImage(image,3,0,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_Y] = GetTileImage(image,4,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Y] = GetTileImage(image,5,0,faceWidth,faceHeight);
					break;

				case CML_HORIZONTALNVIDIA:
					faceWidth = image->GetWidth() / MAX_CUBEMAP_FACES;
					faceHeight = image->GetHeight();
					for(unsigned i = 0; i < MAX_CUBEMAP_FACES; ++i)
						loadImages_[i] = GetTileImage(image,i,0,faceWidth,faceHeight);
					break;

				case CML_HORIZONTALCROSS:
					faceWidth = image->GetWidth() / 4;
					faceHeight = image->GetHeight() / 3;
					loadImages_[FACE_POSITIVE_Y] = GetTileImage(image,1,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_X] = GetTileImage(image,0,1,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_Z] = GetTileImage(image,1,1,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_X] = GetTileImage(image,2,1,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Z] = GetTileImage(image,3,1,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Y] = GetTileImage(image,1,2,faceWidth,faceHeight);
					break;

				case CML_VERTICALCROSS:
					faceWidth = image->GetWidth() / 3;
					faceHeight = image->GetHeight() / 4;
					loadImages_[FACE_POSITIVE_Y] = GetTileImage(image,1,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_X] = GetTileImage(image,0,1,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_Z] = GetTileImage(image,1,1,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_X] = GetTileImage(image,2,1,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Y] = GetTileImage(image,1,2,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Z] = GetTileImage(image,1,3,faceWidth,faceHeight);
					if(loadImages_[FACE_NEGATIVE_Z])
					{
						loadImages_[FACE_NEGATIVE_Z]->FlipVertical();
						loadImages_[FACE_NEGATIVE_Z]->FlipHorizontal();
					}
					break;

				case CML_BLENDER:
					faceWidth = image->GetWidth() / 3;
					faceHeight = image->GetHeight() / 2;
					loadImages_[FACE_NEGATIVE_X] = GetTileImage(image,0,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Z] = GetTileImage(image,1,0,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_X] = GetTileImage(image,2,0,faceWidth,faceHeight);
					loadImages_[FACE_NEGATIVE_Y] = GetTileImage(image,0,1,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_Y] = GetTileImage(image,1,1,faceWidth,faceHeight);
					loadImages_[FACE_POSITIVE_Z] = GetTileImage(image,2,1,faceWidth,faceHeight);
					break;
				}
			}
		}
		// Face per image
		else
		{
			XmlNode faceElem = textureElem.child("faces");
			for(XmlNode faceChild = faceElem.first_child(); faceChild; faceChild = faceChild.next_sibling())
			{
				YumeString name = faceChild.attribute("name").as_string();

				// If path is empty, add the XML file path
				if(GetPath(name).empty())
					name = texPath + name;

				loadImages_.push_back(rm_->GetTempResource<YumeImage>(name));
				rm_->StoreResourceDependency(this,name);
			}
		}

		// Precalculate mip levels if async loading
		if(GetAsyncLoadState() == ASYNC_LOADING)
		{
			for(unsigned i = 0; i < loadImages_.size(); ++i)
			{
				if(loadImages_[i])
					loadImages_[i]->PrecalculateLevels();
			}
		}

		return true;
	}

	bool YumeTextureCube::EndLoad()
	{
		// If over the texture budget, see if materials can be freed to allow textures to be freed
		CheckTextureBudget(GetType());

		SetParameters(loadParameters_);

		for(unsigned i = 0; i < loadImages_.size() && i < MAX_CUBEMAP_FACES; ++i)
			SetData((CubeMapFace)i,loadImages_[i]);

		loadImages_.clear();
		loadParameters_.Reset();

		return true;
	}
}
