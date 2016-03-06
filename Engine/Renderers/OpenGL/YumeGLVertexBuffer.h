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
		/// Construct.
		YumeGLVertexBuffer(YumeRHI* rhi);
		/// Destruct.
		virtual ~YumeGLVertexBuffer();

		void OnDeviceReset();
		/// Release buffer.
		void Release();

		/// Enable shadowing in CPU memory. Shadowing is forced on if the graphics subsystem does not exist.
		void SetShadowed(bool enable);
		/// Set size and vertex elements and dynamic mode. Previous data will be lost.
		bool SetSize(unsigned vertexCount,unsigned elementMask,bool dynamic = false);
		/// Set all data in the buffer.
		bool SetData(const void* data);
		/// Set a data range in the buffer. Optionally discard data outside the range.
		bool SetDataRange(const void* data,unsigned start,unsigned count,bool discard = false);
		/// Lock the buffer for write-only editing. Return data pointer if successful. Optionally discard data outside the range.
		void* Lock(unsigned start,unsigned count,bool discard = false);
		/// Unlock the buffer and apply changes to the GPU buffer.
		void Unlock();

		/// Return whether CPU memory shadowing is enabled.
		bool IsShadowed() const { return shadowed_; }

		/// Return whether is dynamic.
		bool IsDynamic() const { return dynamic_; }

		/// Return whether is currently locked.
		bool IsLocked() const { return lockState_ != LOCK_NONE; }

		/// Return number of vertices.
		unsigned GetVertexCount() const { return vertexCount_; }

		/// Return vertex size.
		unsigned GetVertexSize() const { return vertexSize_; }

		/// Return bitmask of vertex elements.
		unsigned GetElementMask() const { return elementMask_; }

		/// Return offset of a specified element within a vertex.
		unsigned GetElementOffset(VertexElement element) const { return elementOffset_[element]; }

		/// Return CPU memory shadow data.
		unsigned char* GetShadowData() const { return shadowData_.get(); }

		/// Return shared array pointer to the CPU memory shadow data.
		boost::shared_array<unsigned char> GetShadowDataShared() const { return shadowData_; }

		/// Return vertex size corresponding to a vertex element mask.
		static unsigned GetVertexSize(unsigned elementMask);
		/// Return element offset from an element mask.
		static unsigned GetElementOffset(unsigned elementMask,VertexElement element);

		/// Vertex element sizes in bytes.
		static const unsigned elementSize[];
		/// Vertex element OpenGL types.
		static const unsigned elementType[];
		/// Vertex element OpenGL component counts.
		static const unsigned elementComponents[];
		/// Vertex element OpenGL normalization.
		static const unsigned elementNormalize[];

	public:
		/// Update offsets of vertex elements.
		void UpdateOffsets();
		/// Create buffer.
		bool Create();
		/// Update the shadow data to the GPU buffer.
		bool UpdateToGPU();
	};
}


//----------------------------------------------------------------------------
#endif
