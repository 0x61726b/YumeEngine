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
#include "YumeResourceManager.h"
#include "Core/YumeIO.h"
#include "Core/YumeFile.h"
#include "Engine/YumeEngine.h"

#include "Renderer/YumeImage.h"

#include "Core/YumeDefaults.h"

#include "Core/YumeThread.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	YumeResourceManager::YumeResourceManager()
	{
		//backgroundWorker_ = boost::shared_ptr<YumeBackgroundWorker>(YumeAPINew YumeBackgroundWorker(this));
	}

	YumeResourceManager::~YumeResourceManager()
	{
	}

	void YumeResourceManager::AddResourcePath(const FsPath& path)
	{
		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();

		if(!YumeIO::IsDirectoryExist(path))
		{
			YUMELOG_ERROR("Trying to add non-existent resource path " << path);
			return;
		}

		for(int i=0; i < resourcePaths_.size(); ++i)
		{
			if(path.compare(resourcePaths_[i]))
				return;
		}

		resourcePaths_.push_back(path);

		YUMELOG_INFO("Added resource path " << path.generic_string());

	}

	boost::shared_ptr<YumeFile> YumeResourceManager::GetFile(const YumeString& name)
	{
		boost::mutex::scoped_lock lock(resourceMutex_);

		if(name.length() > 0)
		{
			SharedPtr<YumeFile> file = 0;
			file = SearchResourcesPath(name);

			if(file)
				return file;
		}


		return boost::shared_ptr<YumeFile>();
	}

	SharedPtr<YumeResource> YumeResourceManager::RetrieveResource(YumeHash type,const YumeString& resource)
	{
		ResourceGroupHashMap::iterator It = resourceGroups_.find(type);

		if(It != resourceGroups_.end())
		{
			YumeHash nameHash = GenerateHash(resource);

			YumeMap<YumeHash,boost::shared_ptr<YumeResource> >::iterator rgIt = It->second.resources_.find(nameHash);

			if(rgIt != It->second.resources_.end())
			{
				return (rgIt->second);
			}
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}

	bool YumeResourceManager::Exists(const YumeString& name)
	{
		boost::mutex::scoped_lock lock(resourceMutex_);

		if(name.empty())
			return false;

		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();

		for(size_t i = 0; i < resourcePaths_.size(); ++i)
		{
			if(!io_->IsDirectoryExist(resourcePaths_[i] / name))
			{
				return false;
			}
		}
		return true;
	}

	YumeString YumeResourceManager::GetFullPath(const YumeString& resource)
	{
		boost::mutex::scoped_lock lock(resourceMutex_);

		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();

		for(size_t i = 0; i < resourcePaths_.size(); ++i)
		{
			if(io_->IsDirectoryExist(resourcePaths_[i] / resource))
			{
				return (resourcePaths_[i] / resource).generic_string();
			}
		}
		return YumeString();
	}

	void YumeResourceManager::StoreResourceDependency(YumeResource* resource,const YumeString& dep)
	{
		boost::mutex::scoped_lock lock(resourceMutex_);

		if(!resource)
			return;

		YumeHash nameHash = GenerateHash(resource->GetName());
		YumeVector<YumeHash>::type& dependents = dependentResources_[GenerateHash(dep)];
		dependents.push_back(nameHash);
	}

	SharedPtr<YumeResource> YumeResourceManager::PrepareResource(YumeHash type,const YumeString& resource)
	{
		SharedPtr<YumeResource> resourceBase_ = RetrieveResource(type,resource);

		SharedPtr<YumeResource> resource_ = 0;
		if(resourceBase_)
		{
			return resourceBase_;
		}

		YumeHash nameHash = GenerateHash(resource);

		resource_ = boost::static_pointer_cast<YumeResource>(YumeObjectFactory::Get()->Create(type));

		if(!resource_)
		{
			YUMELOG_ERROR("Couldn't create object type " << type);
			return 0;
		}

		SharedPtr<YumeFile> file_ = GetFile(resource);

		if(!file_)
			return 0;

		resource_->SetName(resource);

		YUMELOG_INFO("Loading resource " << resource.c_str() << "...");
		if(!resource_->Load(*(file_.get())))
		{
			//Log error
		}

		resourceGroups_[type].resources_[nameHash] = resource_;

		YUMELOG_INFO("Resource " << resource.c_str() << " is loaded succesfully");

		return resource_;
	}

	SharedPtr<YumeFile> YumeResourceManager::SearchResourcesPath(const YumeString& resource)
	{
		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();

		for(size_t i = 0; i < resourcePaths_.size(); ++i)
		{
			if(io_->IsDirectoryExist(resourcePaths_[i] / resource))
			{
				YumeFile* file = YumeAPINew YumeFile(resourcePaths_[i] / resource);
				file->SetName(resource);
				return boost::shared_ptr<YumeFile>(file);
			}
		}
		return 0;
	}

	bool YumeResourceManager::AddManualResource(YumeResource* resource)
	{
		return false;
	}
}
