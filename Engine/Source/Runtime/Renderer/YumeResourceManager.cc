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

#undef FindResource

namespace YumeEngine
{
	static const SharedPtr<YumeResource> nullResource;

	YumeResourceManager::YumeResourceManager()
	{
		//backgroundWorker_ = SharedPtr<YumeBackgroundWorker>(YumeAPINew YumeBackgroundWorker(this));
	}

	YumeResourceManager::~YumeResourceManager()
	{
	}

	void YumeResourceManager::AddResourcePath(const FsPath& path)
	{
		YumeIO* io_ = gYume->pIO;

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

	SharedPtr<YumeFile> YumeResourceManager::GetFile(const YumeString& name)
	{
		MutexLock lock(resourceMutex_);

		if(name.length() > 0)
		{
			YumeFile* file = 0;
			file = SearchResourcesPath(name);

			if(file)
				return SharedPtr<YumeFile>(file);
		}


		return SharedPtr<YumeFile>();
	}

	void YumeResourceManager::GetResources(YumeVector<YumeResource*>::type& result,YumeHash type) const
	{
		result.clear();
		YumeMap<YumeHash,ResourceGroup>::const_iterator i = resourceGroups_.find(type);
		if(i != resourceGroups_.end())
		{
			for(YumeMap<YumeHash,SharedPtr<YumeResource> >::const_iterator j = i->second.resources_.begin();
				j != i->second.resources_.end(); ++j)
				result.push_back(j->second);
		}
	}

	bool YumeResourceManager::ReloadResource(YumeResource* resource)
	{
		if(!resource)
			return false;

		//Send event

		SharedPtr<YumeFile> file_ = GetFile(resource->GetName());

		if(!file_)
			return 0;

		YUMELOG_INFO("Reloading resource " << resource->GetName().c_str() << "...");
		if(!resource->Load(*(file_)))
		{
			//Log error
		}
		else
		{
			resource->ResetUseTimer();
			UpdateResourceGroup(resource->GetType());
			return true;
		}

		return false;
	}

	YumeResource* YumeResourceManager::RetrieveResource(YumeHash type,const YumeString& resource)
	{
		ResourceGroupHashMap::iterator It = resourceGroups_.find(type);

		if(It != resourceGroups_.end())
		{
			YumeHash nameHash = (resource);

			YumeMap<YumeHash,SharedPtr<YumeResource> >::iterator rgIt = It->second.resources_.find(nameHash);

			if(rgIt != It->second.resources_.end())
			{
				return (rgIt->second);
			}
			else
				return 0;
		}
		else
		{
			return SharedPtr<YumeResource>();
		}
	}

	bool YumeResourceManager::Exists(const YumeString& name)
	{
		MutexLock lock(resourceMutex_);

		if(name.empty())
			return false;

		YumeIO* io_ = gYume->pIO;

		for(size_t i = 0; i < resourcePaths_.size(); ++i)
		{
			if(!io_->IsDirectoryExist(resourcePaths_[i] / name.c_str()))
			{
				return false;
			}
		}
		return true;
	}

	YumeString YumeResourceManager::GetFullPath(const YumeString& resource)
	{
		MutexLock lock(resourceMutex_);

		YumeIO* io_ = gYume->pIO;

		for(size_t i = 0; i < resourcePaths_.size(); ++i)
		{
			if(io_->IsDirectoryExist(resourcePaths_[i] / resource.c_str()))
			{
				return (resourcePaths_[i] / resource.c_str()).generic_string().c_str();
			}
		}
		return YumeString();
	}

	void YumeResourceManager::StoreResourceDependency(YumeResource* resource,const YumeString& dep)
	{
		MutexLock lock(resourceMutex_);

		if(!resource)
			return;

		YumeHash nameHash = (resource->GetName());
		YumeVector<YumeHash>::type& dependents = dependentResources_[(dep)];
		dependents.push_back(nameHash);
	}

	void YumeResourceManager::ResetDependencies(YumeResource* resource)
	{
		if(!resource)
			return;

		MutexLock lock(resourceMutex_);

		YumeHash nameHash(resource->GetName());

		for(YumeMap<YumeHash,YumeVector<YumeHash>::type >::iterator i = dependentResources_.begin(); i != dependentResources_.end();)
		{
			YumeVector<YumeHash>::type& dependents = i->second;
			dependents.erase(nameHash);
			if(dependents.empty())
				dependentResources_.erase(i);
			else
				++i;
		}
	}

	const SharedPtr<YumeResource>& YumeResourceManager::FindResource(YumeHash type,YumeHash nameHash)
	{
		MutexLock lock(resourceMutex_);

		YumeMap<YumeHash,ResourceGroup>::iterator i = resourceGroups_.find(type);
		if(i == resourceGroups_.end())
			return nullResource;
		YumeMap<YumeHash,SharedPtr<YumeResource> >::iterator j = i->second.resources_.find(nameHash);
		if(j == i->second.resources_.end())
			return nullResource;

		return j->second;
	}

	YumeResource* YumeResourceManager::PrepareResource(YumeHash type,const YumeString& resource)
	{
		const SharedPtr<YumeResource>& resourceBase_ = FindResource(type,resource);


		SharedPtr<YumeResource> resource_;
		if(resourceBase_)
		{
			return resourceBase_;
		}

		YumeHash nameHash = (resource);

		resource_ = DynamicCast<YumeResource>(gYume->pObjFactory->Create(type));

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
		if(!resource_->Load(*(file_)))
		{
			//Log error
		}

		resourceGroups_[type].resources_[nameHash] = resource_;

		YUMELOG_INFO("Resource " << resource.c_str() << " is loaded succesfully");
		UpdateResourceGroup(type);
		return resource_;
	}

	SharedPtr<YumeResource> YumeResourceManager::GetTempResource(YumeHash type,const YumeString& resource)
	{
		if(resource.empty())
			return SharedPtr<YumeResource>();

		SharedPtr<YumeResource> resource_;

		resource_ = DynamicCast<YumeResource>(YumeObjectFactory::Get()->Create(type));

		if(!resource_)
		{
			YUMELOG_ERROR("Couldn't create object type " << type);
			return SharedPtr<YumeResource>();
		}

		SharedPtr<YumeFile> file_ = GetFile(resource);

		if(!file_)
			return SharedPtr<YumeResource>();

		YUMELOG_DEBUG("Loading temporary resource " << resource.c_str());
		resource_->SetName(file_->GetName());

		if(!resource_->Load(*(file_)))
		{
			//Log error
			return SharedPtr<YumeResource>();
		}

		return resource_;
	}

	YumeFile* YumeResourceManager::SearchResourcesPath(const YumeString& resource)
	{
		YumeIO* io_ = gYume->pIO;

		for(size_t i = 0; i < resourcePaths_.size(); ++i)
		{
			if(io_->IsDirectoryExist(resourcePaths_[i] / resource.c_str()))
			{
				YumeFile* file = YumeAPINew YumeFile(resourcePaths_[i] / resource.c_str());
				file->SetName(resource);
				return (file);
			}
		}
		return 0;
	}


	void YumeResourceManager::UpdateResourceGroup(YumeHash type)
	{
		YumeMap<YumeHash,ResourceGroup>::iterator i = resourceGroups_.find(type);
		if(i == resourceGroups_.end())
			return;

		for(;;)
		{
			unsigned totalSize = 0;
			unsigned oldestTimer = 0;
			YumeMap<YumeHash,SharedPtr<YumeResource> >::iterator oldestResource = i->second.resources_.end();

			for(YumeMap<YumeHash,SharedPtr<YumeResource> >::iterator j = i->second.resources_.begin();
				j != i->second.resources_.end(); ++j)
			{
				totalSize += j->second->GetMemoryUse();
				unsigned useTimer = j->second->GetUseTimer();
				if(useTimer > oldestTimer)
				{
					oldestTimer = useTimer;
					oldestResource = j;
				}
			}

			i->second.memoryUse_ = totalSize;

			// If memory budget defined and is exceeded, remove the oldest resource and loop again
			// (resources in use always return a zero timer and can not be removed)
			if(i->second.memoryBudget_ && i->second.memoryUse_ > i->second.memoryBudget_ &&
				oldestResource != i->second.resources_.end())
			{
				YUMELOG_DEBUG("Resource group " << oldestResource->second->GetName().c_str() << " over memory budget, releasing resource " <<
					oldestResource->second->GetName().c_str());
				i->second.resources_.erase(oldestResource);
			}
			else
				break;
		}
	}

	bool YumeResourceManager::AddManualResource(YumeResource* resource)
	{
		return false;
	}
}
