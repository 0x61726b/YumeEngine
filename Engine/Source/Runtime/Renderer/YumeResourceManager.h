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
//----------------------------------------------------------------------------
namespace YumeEngine
{
	struct ResourceGroup;

	class YumeImage;

	typedef YumeMap<YumeHash,ResourceGroup>::type ResourceGroupHashMap;
	typedef YumeMap<YumeHash,SharedPtr<YumeResource> >::type ResourceHashMap;

	/// Sets to priority so that a package or file is pushed to the end of the vector.
	static const unsigned PRIORITY_LAST = 0xffffffff;

	/// Container of resources with specific type.
	struct ResourceGroup
	{
		/// Construct with defaults.
		ResourceGroup():
			memoryBudget_(0),
			memoryUse_(0)
		{
		}

		/// Memory budget.
		unsigned long long memoryBudget_;
		/// Current memory use.
		unsigned long long memoryUse_;
		/// Resources.
		YumeMap<YumeHash,boost::shared_ptr<YumeResource> >::type resources_;
	};


	class YumeAPIExport YumeResourceManager
	{
	public:
		YumeResourceManager();

		void AddResourcePath(const FsPath&);

		bool AddManualResource(YumeResource* resource);
		boost::shared_ptr<YumeFile> GetFile(const YumeString& name);

		SharedPtr<YumeFile> SearchResourcesPath(const YumeString& resource);

		SharedPtr<YumeResource> RetrieveResource(YumeHash type,const YumeString& resource);
		
		SharedPtr<YumeImage> GetImage(const YumeString& resource);


		SharedPtr<YumeResource> PrepareResource(YumeHash type,const YumeString& resource);

		template< class T >
		SharedPtr<T> PrepareResource(const YumeString& resource);

	private:
		boost::mutex resourceMutex_;
		ResourceGroupHashMap resourceGroups_;
		YumeVector<FsPath>::type resourcePaths_;
		boost::shared_ptr<YumeBackgroundWorker> backgroundWorker_;


	};

	template< class T > SharedPtr<T> YumeResourceManager::PrepareResource(const YumeString& resource)
	{
		YumeHash type = T::GetType();

		return boost::static_pointer_cast<T>(PrepareResource(type,resource));
	}

}


//----------------------------------------------------------------------------
#endif
