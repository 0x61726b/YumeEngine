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
		backgroundWorker_ = boost::shared_ptr<YumeBackgroundWorker>(YumeAPINew YumeBackgroundWorker(this));

		cachedHashMap_["Base"] = GenerateHash("Base");
		cachedHashMap_["Image"] = GenerateHash("Image");

	}

	void YumeResourceManager::AddResourcePath(const FsPath& path)
	{
		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();

		if(!YumeIO::IsDirectoryExist(path))
		{
			YUMELOG_ERROR("Trying to add non-existent resource path");
		}

		for(int i=0; i < resourcePaths_.size(); ++i)
		{
			if(path.compare(resourcePaths_[i]))
				return;
		}

		resourcePaths_.push_back(path);

		YUMELOG_ERROR("Added resource path " << path.generic_string());

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

	YumeResource* YumeResourceManager::RetrieveResource(YumeHash type,const YumeString& resource)
	{
		ResourceGroupHashMap::iterator It = resourceGroups_.find(type);

		if(It != resourceGroups_.end())
		{
			YumeHash nameHash = GenerateHash(resource);

			YumeMap<YumeHash,boost::shared_ptr<YumeResource> >::iterator rgIt = It->second.resources_.find(nameHash);

			if(rgIt != It->second.resources_.end())
			{
				return (rgIt->second.get());
			}
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}


	SharedPtr<YumeImage> YumeResourceManager::GetImage(const YumeString& resource)
	{
		//Check if resource is loaded before
		YumeHash type = cachedHashMap_["Image"];

		YumeResource* resourceBase_ = RetrieveResource(type,resource);

		SharedPtr<YumeImage> resource_ = 0;
		if(resourceBase_)
		{
			resource_ = SharedPtr<YumeImage>(static_cast<YumeImage*>(resourceBase_));
			return resource_;
		}

		//Resource doesnt exist yet

		YumeHash nameHash = GenerateHash(resource);

		resource_ = boost::shared_ptr<YumeImage>( new YumeImage );

		SharedPtr<YumeFile> file_ = GetFile(resource);

		if(!file_)
			return 0;

		resource_->SetName(resource);

		if(!resource_->Load(*(file_.get())))
		{
			//Log error
		}

		resourceGroups_[type].resources_[nameHash] = resource_;

		return resource_;
	}

	bool YumeResourceManager::PrepareResource(YumeResource* resource)
	{
		//if(!YumeThreadWrapper::IsMainThread())
		//{
		//	YUMELOG_ERROR("Attemp to load resource from non-main thread is ill eagle");
		//	return false;
		//}

		///*backgroundWorker_->WaitForResource(type,resource);*/

		////Check existing resources

		//
		//

		//SharedPtr<YumeFile> file_ = GetFile(resource);

		//if(!file_)
		//	return 0;

		//YUMELOG_INFO("Loading resource " << resource.c_str());

		//resource_->SetName(resource);

		//if(!resource_->Load(*(file_.get())))
		//{
		//	//Send event
		//}

		//YumeHash uniqueHash = resource_->GetHash();
		//YumeHash nameHash = GenerateHash(resource);

		////Cache the resource somehow
		//resource_->ResetUseTimer();
		//resourceGroups_[uniqueHash].resources_[nameHash] = resource_;
		///*UpdateResourceGroup(type);*/

		//return resource_.get();
		return true;
	}

	void YumeResourceManager::UpdateResourceGroup(YumeHash type)
	{
		//ResourceGroupHashMap::iterator i = resourceGroups_.find(type);
		//if(i == resourceGroups_.end())
		//	return;

		//for(;;)
		//{
		//	unsigned totalSize = 0;
		//	unsigned oldestTimer = 0;
		//	ResourceHashMap::iterator oldestResource = i->second.resources_.end();

		//	for(ResourceHashMap::iterator j = i->second.resources_.begin();
		//		j != i->second.resources_.end(); ++j)
		//	{
		//		totalSize += j->second->GetMemoryUse();
		//		unsigned useTimer = j->second->GetUseTimer();
		//		if(useTimer > oldestTimer)
		//		{
		//			oldestTimer = useTimer;
		//			oldestResource = j;
		//		}
		//	}

		//	i->second.memoryUse_ = totalSize;

		//	// If memory budget defined and is exceeded, remove the oldest resource and loop again
		//	// (resources in use always return a zero timer and can not be removed)
		//	if(i->second.memoryBudget_ && i->second.memoryUse_ > i->second.memoryBudget_ &&
		//		oldestResource != i->second.resources_.end())
		//	{
		//		YUMELOG_WARN("Resource group " + oldestResource->second->GetTypeName() + " over memory budget, releasing resource " +
		//			oldestResource->second->GetName());

		//		i->second.resources_.erase(oldestResource);
		//	}
		//	else
		//		break;
		//}
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
