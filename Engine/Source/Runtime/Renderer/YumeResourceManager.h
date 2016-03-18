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
#ifndef __YumeResourceManager_h__
#define __YumeResourceManager_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeBackgroundWorker.h"
#include "YumeResource.h"

#include "YumeIO.h"
#include "Core/YumeMutex.h"
#undef FindResource
//----------------------------------------------------------------------------
namespace YumeEngine
{
	struct ResourceGroup;

	class YumeImage;

	typedef YumeMap<YumeHash,ResourceGroup>::type ResourceGroupHashMap;
	typedef YumeMap<YumeHash,SharedPtr<YumeResource> >::type ResourceHashMap;


	static const unsigned PRIORITY_LAST = 0xffffffff;


	struct ResourceGroup
	{
		ResourceGroup():
			memoryBudget_(0),
			memoryUse_(0)
		{
		}

		unsigned long long memoryBudget_;
		unsigned long long memoryUse_;
		YumeMap<YumeHash,SharedPtr<YumeResource> >::type resources_;
	};


	class YumeAPIExport YumeResourceManager : public YumeBase
	{
	public:
		YumeResourceManager();
		virtual ~YumeResourceManager();

		void AddResourcePath(const FsPath&);

		bool AddManualResource(YumeResource* resource);
		SharedPtr<YumeFile> GetFile(const YumeString& name);

		YumeString GetFullPath(const YumeString& resource);

		YumeFile* SearchResourcesPath(const YumeString& resource);
		YumeResource* RetrieveResource(YumeHash type,const YumeString& resource);
		YumeResource* PrepareResource(YumeHash type,const YumeString& resource);
		void StoreResourceDependency(YumeResource* resource,const YumeString& dep);
		SharedPtr<YumeResource> GetTempResource(YumeHash type,const YumeString& name);
		void GetResources(YumeVector<YumeResource*>::type& result,YumeHash type) const;
		const SharedPtr<YumeResource>& FindResource(YumeHash type, YumeHash nameHash);

		bool ReloadResource(YumeResource* resource);
		void ResetDependencies(YumeResource*);
		bool Exists(const YumeString& name);

		void UpdateResourceGroup(YumeHash type);

		template <class T> T* PrepareResource(const YumeString& resource);
		template <class T> T* RetrieveResource(const YumeString& name);
		template <class T> SharedPtr<T> GetTempResource(const YumeString& name);
		template <class T> void GetResources(eastl::vector<T*>& result) const;


	private:
		Mutex resourceMutex_;
		ResourceGroupHashMap resourceGroups_;
		YumeVector<FsPath>::type resourcePaths_;
		SharedPtr<YumeBackgroundWorker> backgroundWorker_;

		YumeMap<YumeHash,YumeVector<YumeHash>::type >::type dependentResources_;


	};


	template <class T> void YumeResourceManager::GetResources(eastl::vector<T*>& result) const
	{
		YumeVector<YumeResource*>::type& resources = reinterpret_cast<eastl::vector<YumeResource*>&>(result);
		YumeHash type = T::GetTypeStatic();
		GetResources(resources,type);

		// Perform conversion of the returned pointers
		for(unsigned i = 0; i < result.size(); ++i)
		{
			YumeResource* resource = resources[i];
			result[i] = static_cast<T*>(resource);
		}
	}

	template <class T> T* YumeResourceManager::RetrieveResource(const YumeString& name)
	{
		YumeHash type = T::GetTypeStatic();
		return static_cast<T*>(RetrieveResource(type,name));
	}

	template< class T > T* YumeResourceManager::PrepareResource(const YumeString& resource)
	{
		YumeHash type = T::GetTypeStatic();

		return static_cast<T*>(PrepareResource(type,resource));
	}

	template <class T> SharedPtr<T> YumeResourceManager::GetTempResource(const YumeString& name)
	{
		YumeHash type = T::GetTypeStatic();
		return StaticCast<T>(GetTempResource(type,name));
	}

}


//----------------------------------------------------------------------------
#endif
