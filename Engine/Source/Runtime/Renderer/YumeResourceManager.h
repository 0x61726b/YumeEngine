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

#include "YumeResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{


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
		YumeMap<YumeHash,boost::shared_ptr<YumeResource> > resources_;
	};

	/// Resource request types.
	enum ResourceRequest
	{
		RESOURCE_CHECKEXISTS = 0,
		RESOURCE_GETFILE = 1
	};

	/// Optional resource request processor. Can deny requests, re-route resource file names, or perform other processing per request.
	class YumeAPIExport ResourceRouter
	{
	public:
		/// Construct.
		ResourceRouter()
		{
		}

		/// Process the resource request and optionally modify the resource name string. Empty name string means the resource is not found or not allowed.
		virtual void Route(YumeString& name,ResourceRequest requestType) = 0;
	};


	class YumeAPIExport YumeResourceManager
	{
	public:
		YumeResourceManager();

		bool AddManualResource(YumeResource* resource);

		boost::shared_ptr<YumeFile> GetFile(const YumeString& name,bool sendEventOnFailure = true);

		bool GetReturnFailedResources() const { return false; }

	};
}


//----------------------------------------------------------------------------
#endif
