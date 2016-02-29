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
#include "YumeD3D11Required.h"

#include "Renderer/YumeGpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3D11Renderer;
	/// Base class for GPU resources.
	class YumeD3DExport YumeD3D11Resource : public YumeGpuResource
	{
	public:
		YumeD3D11Resource(YumeD3D11Renderer*);
		/// Destruct. Remove from the graphics subsystem.
		virtual ~YumeD3D11Resource();

		/// Unconditionally release the GPU resource.
		virtual void Release() { }

		/// Clear the data lost flag. No-op on D3D11.
		void ClearDataLost() { }

		/// Return whether data is lost due to device loss. Always false on D3D11.
		bool IsDataLost() const { return false; }

		/// Return whether has pending data assigned while device was lost. Always false on D3D11.
		bool HasPendingData() const { return false; }
	};
}


//----------------------------------------------------------------------------
#endif
