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
#include "YumeD3D11GpuResource.h"
#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeVertexBuffer.h"

#include <boost/shared_array.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Hardware vertex buffer.
	class YumeD3DExport YumeD3D11VertexBuffer :public YumeVertexBuffer,public YumeD3D11Resource
	{
	public:
		/// Construct.
		YumeD3D11VertexBuffer(YumeRHI* rhi);
		/// Destruct.
		virtual ~YumeD3D11VertexBuffer();

		/// Release buffer.
		virtual void Release();

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

		/// Return buffer hash for building vertex declarations.
		unsigned long long GetBufferHash(unsigned streamIndex,unsigned useMask);

		/// Return CPU memory shadow data.
		unsigned char* GetShadowData() const { return shadowData_.get(); }

		/// Return shared array pointer to the CPU memory shadow data.
		boost::shared_array<unsigned char> GetShadowDataShared() const { return shadowData_; }

		/// Return vertex size corresponding to a vertex element mask.
		static unsigned GetVertexSize(unsigned elementMask);
		/// Return element offset from an element mask.
		static unsigned GetElementOffset(unsigned elementMask,VertexElement element);

		/// Vertex element semantic names.
		static const char* elementSemantics[];
		/// Vertex element semantic indices.
		static const unsigned elementSemanticIndices[];
		/// Vertex element formats.
		static const unsigned elementFormats[];

	public:
		/// Update offsets of vertex elements.
		void UpdateOffsets();
		/// Create buffer.
		bool Create();
		/// Update the shadow data to the GPU buffer.
		bool UpdateToGPU();
	private:
		/// Map the GPU buffer into CPU memory.
		void* MapBuffer(unsigned start,unsigned count,bool discard);
		/// Unmap the GPU buffer.
		void UnmapBuffer();
	};
}


//----------------------------------------------------------------------------
#endif
