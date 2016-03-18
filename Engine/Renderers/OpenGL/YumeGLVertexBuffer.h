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
#ifndef __YumeD3D11VertexBuffer_h__
#define __YumeD3D11VertexBuffer_h__
//----------------------------------------------------------------------------
#include "YumeGLGpuResource.h"
#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeVertexBuffer.h"

#include <boost/shared_array.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Hardware vertex buffer.
	class YumeGLExport YumeGLVertexBuffer :public YumeVertexBuffer,public YumeGLResource
	{
	public:
		YumeGLVertexBuffer();
		virtual ~YumeGLVertexBuffer();

		void OnDeviceReset();
		void Release();

		void SetShadowed(bool enable);
		bool SetSize(unsigned vertexCount,unsigned elementMask,bool dynamic = false);
		bool SetData(const void* data);
		bool SetDataRange(const void* data,unsigned start,unsigned count,bool discard = false);
		void* Lock(unsigned start,unsigned count,bool discard = false);

		void Unlock();

		bool IsShadowed() const { return shadowed_; }
		bool IsDynamic() const { return dynamic_; }
		bool IsLocked() const { return lockState_ != LOCK_NONE; }

		
		unsigned GetVertexCount() const { return vertexCount_; }
		unsigned GetVertexSize() const { return vertexSize_; }
		unsigned GetElementMask() const { return elementMask_; }
		unsigned GetElementOffset(VertexElement element) const { return elementOffset_[element]; }

		unsigned char* GetShadowData() const { return shadowData_.get(); }
		boost::shared_array<unsigned char> GetShadowDataShared() const { return shadowData_; }

		static unsigned GetVertexSize(unsigned elementMask);
		static unsigned GetElementOffset(unsigned elementMask,VertexElement element);

		static const unsigned elementSize[];
		static const unsigned elementType[];
		static const unsigned elementComponents[];
		static const unsigned elementNormalize[];

	public:
		void UpdateOffsets();
		bool Create();
		bool UpdateToGPU();
	};
}


//----------------------------------------------------------------------------
#endif
