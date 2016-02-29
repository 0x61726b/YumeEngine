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
#ifndef __YumeVertexBuffer_h__
#define __YumeVertexBuffer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeGpuResource.h"

#include <boost/shared_array.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeVertexBuffer
	{
	public:
		YumeVertexBuffer();

		virtual ~YumeVertexBuffer();

		virtual void Release() = 0;

		virtual void SetShadowed(bool enable) = 0;
		virtual bool SetSize(unsigned vertexCount,unsigned elementMask,bool dynamic = false) = 0;
		virtual bool SetData(const void* data) = 0;
		virtual bool SetDataRange(const void* data,unsigned start,unsigned count,bool discard = false) = 0;
		virtual void* Lock(unsigned start,unsigned count,bool discard = false) = 0;
		virtual void Unlock() = 0;


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



		virtual void UpdateOffsets() = 0;
		virtual bool Create() = 0;
		virtual bool UpdateToGPU() = 0;

		/// Vertex element sizes in bytes.
		static const unsigned elementSize[];
	protected:
		/// Shadow data.
		boost::shared_array<unsigned char> shadowData_;
		/// Number of vertices.
		unsigned vertexCount_;
		/// Vertex size.
		unsigned vertexSize_;
		/// Vertex element bitmask.
		unsigned elementMask_;
		/// Vertex element offsets.
		unsigned elementOffset_[MAX_VERTEX_ELEMENTS];
		/// Buffer locking state.
		LockState lockState_;
		/// Lock start vertex.
		unsigned lockStart_;
		/// Lock number of vertices.
		unsigned lockCount_;
		/// Scratch buffer for fallback locking.
		void* lockScratchData_;
		/// Shadowed flag.
		bool shadowed_;
		/// Dynamic flag.
		bool dynamic_;
	};
}


//----------------------------------------------------------------------------
#endif
