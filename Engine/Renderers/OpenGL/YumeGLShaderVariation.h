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
#ifndef __YumeD3D11ShaderVariation_h__
#define __YumeD3D11ShaderVariation_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"

#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeShaderVariation.h"
#include "YumeGLGpuResource.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeShader;
	class YumeConstantBuffer;

	/// %Shader parameter definition.
	struct ShaderParameter
	{
		/// Construct with defaults.
		ShaderParameter():
			bufferPtr_(0)
		{
		}

		/// Uniform location or byte offset in constant buffer.
		int location_;
		/// Element type.
		unsigned type_;
		/// Constant buffer pointer.
		YumeConstantBuffer* bufferPtr_;
	};

	/// Vertex or pixel shader on the GPU.
	class YumeGLExport YumeGLShaderVariation : public YumeShaderVariation,public YumeGLResource
	{
	public:
		/// Construct.
		YumeGLShaderVariation(YumeShader* owner,ShaderType type);
		/// Destruct.
		virtual ~YumeGLShaderVariation();

		void OnDeviceLost();

		/// Release the shader.
		virtual void Release();

		/// Compile the shader. Return true if successful.
		virtual bool Create();

		const YumeMap<YumeHash,ShaderParameter>::type& GetParameters() const { YumeMap<YumeHash,ShaderParameter>::type a; return a; };
		const unsigned* GetConstantBufferSizes() const { return 0; }
	};
}


//----------------------------------------------------------------------------
#endif
