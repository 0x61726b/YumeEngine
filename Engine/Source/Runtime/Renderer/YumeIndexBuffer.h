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
#ifndef __YumeIndexBuffer_h__
#define __YumeIndexBuffer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeGpuResource.h"
#include "Core/YumeBase.h"

#include <boost/shared_array.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeIndexBuffer : public YumeBase
	{

	public:
		YumeIndexBuffer();
		virtual ~YumeIndexBuffer();
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
		unsigned GetIndexCount() const { return indexCount_; }
		unsigned GetIndexSize() const { return indexSize_; }

		virtual bool GetUsedVertexRange(unsigned start,unsigned count,unsigned& minVertex,unsigned& vertexCount) = 0;
		unsigned char* GetShadowData() const { return shadowData_.get(); }

		boost::shared_array<unsigned char> GetShadowDataShared() const { return shadowData_; }

		virtual bool Create() = 0;
		virtual bool UpdateToGPU() = 0;
		virtual void* MapBuffer(unsigned start,unsigned count,bool discard) = 0;
		virtual void UnmapBuffer() = 0;
	protected:
		/// Shadow data.
		boost::shared_array<unsigned char> shadowData_;
		/// Number of indices.
		unsigned indexCount_;
		/// Index size.
		unsigned indexSize_;
		/// Buffer locking state.
		LockState lockState_;
		/// Lock start vertex.
		unsigned lockStart_;
		/// Lock number of vertices.
		unsigned lockCount_;
		/// Scratch buffer for fallback locking.
		void* lockScratchData_;
		/// Dynamic flag.
		bool dynamic_;
		/// Shadowed flag.
		bool shadowed_;
	};
}



//----------------------------------------------------------------------------
#endif
