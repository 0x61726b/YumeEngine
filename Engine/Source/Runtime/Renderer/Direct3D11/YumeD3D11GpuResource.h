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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeD3D11GpuResource_h__
#define __YumeD3D11GpuResource_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <boost/weak_ptr.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeGraphics;

	/// Base class for GPU resources.
	class YumeAPIExport YumeGpuResource
	{
	public:
		YumeGpuResource();
		/// Destruct. Remove from the graphics subsystem.
		virtual ~YumeGpuResource();

		/// Unconditionally release the GPU resource.
		virtual void Release() { }

		/// Clear the data lost flag. No-op on D3D11.
		void ClearDataLost() { }

		/// Return Direct3D object.
		void* GetGPUObject() const { return object_; }

		/// Return whether data is lost due to device loss. Always false on D3D11.
		bool IsDataLost() const { return false; }

		/// Return whether has pending data assigned while device was lost. Always false on D3D11.
		bool HasPendingData() const { return false; }

		/// Direct3D object.
		void* object_;
	};
}


//----------------------------------------------------------------------------
#endif
