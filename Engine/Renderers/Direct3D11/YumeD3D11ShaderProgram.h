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
#ifndef __YumeShaderProgram_h__
#define __YumeShaderProgram_h__
//----------------------------------------------------------------------------
#include "YumeD3D11Required.h"

#include "Renderer/YumeRHI.h"
#include "Renderer/YumeRendererDefs.h"
#include "YumeD3D11ShaderVariation.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3D11ConstantBuffer;
	/// Combined information for specific vertex and pixel shaders.
	class YumeD3DExport YumeD3D11ShaderProgram : public YumeBase
	{
	public:
		/// Construct.
		YumeD3D11ShaderProgram(YumeRHI* graphics,YumeShaderVariation* vertexShader,YumeShaderVariation* pixelShader)
		{
			// Create needed constant buffers
			const unsigned* vsBufferSizes = vertexShader->GetConstantBufferSizes();
			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			{
				if(vsBufferSizes[i])
					vsConstantBuffers_[i] = graphics->GetOrCreateConstantBuffer(VS,i,vsBufferSizes[i]);
				else
					vsConstantBuffers_[i] = 0;
			}

			const unsigned* psBufferSizes = pixelShader->GetConstantBufferSizes();
			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			{
				if(psBufferSizes[i])
					psConstantBuffers_[i] = graphics->GetOrCreateConstantBuffer(PS,i,psBufferSizes[i]);
				else
					psConstantBuffers_[i] = 0;
			}

			//if(geometryShader)
			//{
			//	const unsigned* gsBufferSizes = geometryShader->GetConstantBufferSizes();
			//	for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			//	{
			//		if(gsBufferSizes[i])
			//			gsConstantBuffers_[i] = graphics->GetOrCreateConstantBuffer(GS,i,gsBufferSizes[i]);
			//		else
			//			gsConstantBuffers_[i] = 0;
			//	}

			//	const YumeMap<YumeHash,ShaderParameter>::type& gsParams = static_cast<YumeD3D11ShaderVariation*>(geometryShader)->GetParameters();
			//	for(YumeMap<YumeHash,ShaderParameter>::const_iterator i = gsParams.begin(); i != gsParams.end(); ++i)
			//	{
			//		parameters_[i->first] = i->second;
			//		parameters_[i->first].bufferPtr_ = gsConstantBuffers_[i->second.buffer_];
			//	}
			//}
			// Copy parameters. Add direct links to constant buffers.
			const YumeMap<YumeHash,ShaderParameter>::type& vsParams = static_cast<YumeD3D11ShaderVariation*>(vertexShader)->GetParameters();
			for(YumeMap<YumeHash,ShaderParameter>::const_iterator i = vsParams.begin(); i != vsParams.end(); ++i)
			{
				parameters_[i->first] = i->second;
				parameters_[i->first].bufferPtr_ = vsConstantBuffers_[i->second.buffer_];
			}

			const YumeMap<YumeHash,ShaderParameter>::type& psParams = static_cast<YumeD3D11ShaderVariation*>(pixelShader)->GetParameters();
			for(YumeMap<YumeHash,ShaderParameter>::const_iterator i = psParams.begin(); i != psParams.end(); ++i)
			{
				parameters_[i->first] = i->second;
				parameters_[i->first].bufferPtr_ = psConstantBuffers_[i->second.buffer_];
			}


			parameters_.Rehash(NextPowerOfTwo(parameters_.size()));
		}

		/// Destruct.
		~YumeD3D11ShaderProgram()
		{
		}

		YumeMap<YumeHash,ShaderParameter>::type parameters_;
		YumeConstantBuffer* vsConstantBuffers_[MAX_SHADER_PARAMETER_GROUPS];
		YumeConstantBuffer* psConstantBuffers_[MAX_SHADER_PARAMETER_GROUPS];
		/*YumeConstantBuffer* gsConstantBuffers_[MAX_SHADER_PARAMETER_GROUPS];*/
	};
}


//----------------------------------------------------------------------------
#endif
