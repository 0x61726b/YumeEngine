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
#include "YumeGLRequired.h"

#include "Renderer/YumeRHI.h"
#include "Renderer/YumeRendererDefs.h"
#include "YumeGLShaderVariation.h"
#include "YumeGLShader.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Combined information for specific vertex and pixel shaders.
	class YumeGLExport YumeGLShaderProgram : public YumeGLResource
	{
	public:
		/// Construct.
		YumeGLShaderProgram(YumeShaderVariation* vertexShader,YumeShaderVariation* pixelShader);

		/// Destruct.
		~YumeGLShaderProgram();
		/// Mark the GPU resource destroyed on context destruction.
		virtual void OnDeviceLost();
		/// Release shader program.
		virtual void Release();

		/// Link the shaders and examine the uniforms and samplers used. Return true if successful.
		bool Link();

		/// Return the vertex shader.
		YumeShaderVariation* GetVertexShader() const;
		/// Return the pixel shader.
		YumeShaderVariation* GetPixelShader() const;
		/// Return whether uses a shader parameter.
		bool HasParameter(YumeHash param) const;

		/// Return whether uses a texture unit.
		bool HasTextureUnit(TextureUnit unit) const { return useTextureUnit_[unit]; }

		/// Return the info for a shader parameter, or null if does not exist.
		const ShaderParameter* GetParameter(YumeHash param) const;

		/// Return linker output.
		const YumeString& GetLinkerOutput() const { return linkerOutput_; }

		/// Return all constant buffers.
		const SharedPtr<YumeConstantBuffer>* GetConstantBuffers() const { return &constantBuffers_[0]; }

		/// Check whether a shader parameter group needs update. Does not actually check whether parameters exist in the shaders.
		bool NeedParameterUpdate(ShaderParameterGroup group,const void* source);
		/// Clear a parameter source. Affects only the current shader program if appropriate.
		void ClearParameterSource(ShaderParameterGroup group);

		/// Clear all parameter sources from all shader programs by incrementing the global parameter source framenumber.
		static void ClearParameterSources();
		/// Clear a global parameter source when constant buffers change.
		static void ClearGlobalParameterSource(ShaderParameterGroup group);

	private:
		/// Vertex shader.
		YumeShaderVariation* vertexShader_;
		/// Pixel shader.
		YumeShaderVariation* pixelShader_;
		/// Shader parameters.
		YumeMap<YumeHash,ShaderParameter>::type shaderParameters_;
		/// Texture unit use.
		bool useTextureUnit_[MAX_TEXTURE_UNITS];
		/// Constant buffers by binding index.
		SharedPtr<YumeConstantBuffer> constantBuffers_[MAX_SHADER_PARAMETER_GROUPS * 2];
		/// Remembered shader parameter sources for individual uniform mode.
		const void* parameterSources_[MAX_SHADER_PARAMETER_GROUPS];
		/// Shader link error string.
		YumeString linkerOutput_;
		/// Shader parameter source framenumber.
		unsigned frameNumber_;

		/// Global shader parameter source framenumber.
		static unsigned globalFrameNumber;
		/// Remembered global shader parameter sources for constant buffer mode.
		static const void* globalParameterSources[MAX_SHADER_PARAMETER_GROUPS];
	};
}


//----------------------------------------------------------------------------
#endif
