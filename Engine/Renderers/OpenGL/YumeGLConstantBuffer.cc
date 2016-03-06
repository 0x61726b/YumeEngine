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
#include "YumeHeaders.h"
#include "YumeGLConstantBuffer.h"
#include "YumeGLRendererImpl.h"
#include "YumeGLRenderer.h"
#include "Engine/YumeEngine.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	YumeGLConstantBuffer::YumeGLConstantBuffer(YumeRHI* impl)
		: YumeGLResource(impl)
	{
	}

	YumeGLConstantBuffer::~YumeGLConstantBuffer()
	{
		Release();
	}

	void YumeGLConstantBuffer::Release()
	{
		if(object_)
		{
			if(!rhi_)
				return;

#ifndef GL_ES_VERSION_2_0
			static_cast<YumeGLRenderer*>(rhi_)->SetUBO(0);
			glDeleteBuffers(1,(GLuint*)&object_);
#endif
			object_ = 0;
		}

		shadowData_.reset();
		size_ = 0;
	}

	void YumeGLConstantBuffer::OnDeviceReset()
	{
		if(size_)
			SetSize(size_); // Recreate
	}

	bool YumeGLConstantBuffer::SetSize(unsigned size)
	{
		Release();

		if(!size)
		{
			YUMELOG_ERROR("Can not create zero-sized constant buffer");
			return false;
		}

		// Round up to next 16 bytes
		size += 15;
		size &= 0xfffffff0;

		size_ = size;
		dirty_ = false;
		shadowData_ = boost::shared_array<unsigned char>(new unsigned char[size_]);
		memset(shadowData_.get(),0,size_);

		if(rhi_)
		{
#ifndef GL_ES_VERSION_2_0
			if(!object_)
			{
				glGenBuffers(1,(GLuint*)object_);
			}
			static_cast<YumeGLRenderer*>(rhi_)->SetUBO(object_);
			glBufferData(GL_UNIFORM_BUFFER,size_,shadowData_.get(),GL_DYNAMIC_DRAW);
#endif
		}

		return true;
	}

	void YumeGLConstantBuffer::Apply()
	{
		if(dirty_ && object_)
		{
#ifndef GL_ES_VERSION_2_0
			static_cast<YumeGLRenderer*>(rhi_)->SetUBO(object_);
			glBufferData(GL_UNIFORM_BUFFER,size_,shadowData_.get(),GL_DYNAMIC_DRAW);
#endif
			dirty_ = false;
		}
	}
}
