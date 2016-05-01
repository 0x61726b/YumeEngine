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
#include "Scene.h"
#include "SceneNode.h"
#include "Light.h"



namespace YumeEngine
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		nodes_.clear();
	}

	SceneNodes::type Scene::GetRenderables()
	{
		renderables_.clear();

		for(int i=0; i < nodes_.size(); ++i)
		{
			if(nodes_[i]->GetType() == GT_STATIC)
				renderables_.push_back(nodes_[i]);
		}
		return renderables_;
	}

	SceneNodes::type Scene::GetLights()
	{
		lights_.clear();

		for(int i=0; i < nodes_.size(); ++i)
		{
			if(nodes_[i]->GetType() == LT_POINT)
				lights_.push_back(nodes_[i]);
		}
		return lights_;
	}

	SceneNode* Scene::GetDirectionalLight()
	{
		SceneNode* light = 0;
		for(int i=0; i < nodes_.size(); ++i)
		{
			if(nodes_[i]->GetType() == GT_LIGHT)
			{
				Light* l = static_cast<Light*>(nodes_[i]);

				if(l->GetType() == LT_DIRECTIONAL)
					light = l;
			}
		}
		return light;
	}

	void Scene::AddNode(SceneNode* node)
	{
		nodes_.push_back(node);
	}
}
