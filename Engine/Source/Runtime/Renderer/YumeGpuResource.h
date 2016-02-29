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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeGpuResource_h__
#define __YumeGpuResource_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeRenderer;
	class YumeEngine3D;

	/// Base class for GPU resources.
	class YumeAPIExport YumeGpuResource
	{
	public:
		YumeGpuResource(YumeRenderer*);
		/// Destruct. Remove from the graphics subsystem.
		virtual ~YumeGpuResource();

		/// Unconditionally release the GPU resource.
		virtual void Release() {};

		/// Clear the data lost flag. No-op on D3D11.
		virtual void ClearDataLost() = 0;

		/// Return Direct3D object.
		void* GetGPUObject() const { return object_; }

		/// Return whether data is lost due to device loss. Always false on D3D11.
		virtual bool IsDataLost() const = 0;

		/// Return whether has pending data assigned while device was lost. Always false on D3D11.
		virtual bool HasPendingData() const = 0;

		void* object_;

	protected:
		YumeRenderer* rhi_;
	};
}


//----------------------------------------------------------------------------
#endif
