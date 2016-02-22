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
#ifndef __YumeGLGpuResource_h__
#define __YumeGLGpuResource_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"

#include "Renderer/YumeGpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeGLExport YumeGLGpuResource : public YumeGpuResource
	{
	public:
		YumeGLGpuResource();
		/// Destruct. Remove from the graphics subsystem.
		virtual ~YumeGLGpuResource();

		/// Unconditionally release the GPU resource.
		virtual void Release() { }

		/// Clear the data lost flag. No-op on D3D11.
		void ClearDataLost() { }

		/// Return Direct3D object.
		void* GetGPUObject() const { return object_; }

		/// Return whether data is lost due to device loss. Always false on D3D11.
		bool IsDataLost() const { return dataLost_; }

		/// Return whether has pending data assigned while device was lost. Always false on D3D11.
		bool HasPendingData() const { return dataPending_; }


  protected:
    bool dataLost_;
    bool dataPending_;

    void* object_;

	};
}


//----------------------------------------------------------------------------
#endif
