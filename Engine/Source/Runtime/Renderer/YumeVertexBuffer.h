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
#include "Core/YumeBase.h"

#include <boost/shared_array.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeVertexBuffer : public YumeBase
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

		void SetVertexSize(unsigned size) { vertexSize_ = size;}

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

		
		static const unsigned elementSize[];
	protected:
		
		boost::shared_array<unsigned char> shadowData_;
		
		unsigned vertexCount_;
		
		unsigned vertexSize_;
		
		unsigned elementMask_;
		
		unsigned elementOffset_[MAX_VERTEX_ELEMENTS];
		
		LockState lockState_;
		
		unsigned lockStart_;
		
		unsigned lockCount_;
		
		void* lockScratchData_;
		
		bool shadowed_;
		
		bool dynamic_;
	};
}


//----------------------------------------------------------------------------
#endif
