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
#include "YumeHeaders.h"
#include "YumeGLShaderProgram.h"
#include "YumeGLConstantBuffer.h"
#include "YumeGLVertexBuffer.h"
#include "YumeGLRenderer.h"

#include "Core/YumeDefaults.h"

#include "Logging/logging.h"
namespace YumeEngine
{

	const char* shaderParameterGroups[] ={
		"frame",
		"camera",
		"zone",
		"light",
		"material",
		"object",
		"custom"
	};

	unsigned YumeGLShaderProgram::globalFrameNumber = 0;
	const void* YumeGLShaderProgram::globalParameterSources[MAX_SHADER_PARAMETER_GROUPS];


	YumeGLShaderProgram::YumeGLShaderProgram(YumeRHI* graphics,YumeShaderVariation* vertexShader,YumeShaderVariation* pixelShader):
		YumeGLResource(graphics),
		vertexShader_(vertexShader),
		pixelShader_(pixelShader),
		frameNumber_(0)
	{
		for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
			useTextureUnit_[i] = false;
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			parameterSources_[i] = (const void*)M_MAX_UNSIGNED;
	}

	YumeGLShaderProgram::~YumeGLShaderProgram()
	{
		Release();
	}

	void YumeGLShaderProgram::OnDeviceLost()
	{
		YumeGLResource::OnDeviceLost();

		if(rhi_ && static_cast<YumeGLRenderer*>(rhi_)->GetShaderProgram() == this)
			rhi_->SetShaders(0,0);

		linkerOutput_.clear();
	}

	void YumeGLShaderProgram::Release()
	{
		if(object_)
		{
			if(!rhi_)
				return;

			if(!rhi_->IsDeviceLost())
			{
				if(static_cast<YumeGLRenderer*>(rhi_)->GetShaderProgram() == this)
					rhi_->SetShaders(0,0);

				glDeleteProgram(object_);
			}

			object_ = 0;
			linkerOutput_.clear();
			shaderParameters_.clear();

			for(unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
				useTextureUnit_[i] = false;
			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
				constantBuffers_[i].reset();
		}
	}

	bool YumeGLShaderProgram::Link()
	{
		Release();

		if(!vertexShader_ || !pixelShader_ || !static_cast<YumeGLShaderVariation*>(vertexShader_)->GetGPUObject() || !static_cast<YumeGLShaderVariation*>(pixelShader_)->GetGPUObject())
			return false;

		object_ = glCreateProgram();
		if(!object_)
		{
			linkerOutput_ = "Could not create shader program";
			return false;
		}

		// Bind vertex attribute locations to ensure they are the same in all shaders
		// Note: this is not the same order as in VertexBuffer, instead a remapping is used to ensure everything except cube texture
		// coordinates fit to the first 8 for better GLES2 device compatibility
		glBindAttribLocation(object_,0,"iPos");
		glBindAttribLocation(object_,1,"iNormal");
		glBindAttribLocation(object_,2,"iColor");
		glBindAttribLocation(object_,3,"iTexCoord");
		glBindAttribLocation(object_,4,"iTexCoord2");
		glBindAttribLocation(object_,5,"iTangent");
		glBindAttribLocation(object_,6,"iBlendWeights");
		glBindAttribLocation(object_,7,"iBlendIndices");
		glBindAttribLocation(object_,8,"iCubeTexCoord");
		glBindAttribLocation(object_,9,"iCubeTexCoord2");
#if !defined(GL_ES_VERSION_2_0) || defined(__EMSCRIPTEN__)
		glBindAttribLocation(object_,10,"iInstanceMatrix1");
		glBindAttribLocation(object_,11,"iInstanceMatrix2");
		glBindAttribLocation(object_,12,"iInstanceMatrix3");
#endif
		glBindAttribLocation(object_,13,"iObjectIndex");

		glAttachShader(object_,static_cast<YumeGLShaderVariation*>(vertexShader_)->GetGPUObject());
		glAttachShader(object_,static_cast<YumeGLShaderVariation*>(pixelShader_)->GetGPUObject());
		glLinkProgram(object_);

		int linked,length;
		glGetProgramiv(object_,GL_LINK_STATUS,&linked);
		if(!linked)
		{
			glGetProgramiv(object_,GL_INFO_LOG_LENGTH,&length);
			linkerOutput_.resize(length);
			int outLength;
			glGetProgramInfoLog(object_,length,&outLength,&linkerOutput_[0]);
			glDeleteProgram(object_);
			object_ = 0;
		}
		else
			linkerOutput_.clear();

		if(!object_)
			return false;

		const int MAX_PARAMETER_NAME_LENGTH = 256;
		char uniformName[MAX_PARAMETER_NAME_LENGTH];
		int uniformCount;

		glUseProgram(object_);
		glGetProgramiv(object_,GL_ACTIVE_UNIFORMS,&uniformCount);

		// Check for constant buffers
#ifndef GL_ES_VERSION_2_0
		YumeMap<unsigned,unsigned>::type blockToBinding;

		if(YumeGLRenderer::GetGL3Support())
		{
			int numUniformBlocks = 0;

			glGetProgramiv(object_,GL_ACTIVE_UNIFORM_BLOCKS,&numUniformBlocks);
			for(int i = 0; i < numUniformBlocks; ++i)
			{
				int nameLength;
				glGetActiveUniformBlockName(object_,(GLuint)i,MAX_PARAMETER_NAME_LENGTH,&nameLength,uniformName);

				YumeString name(uniformName,nameLength);

				unsigned blockIndex = glGetUniformBlockIndex(object_,name.c_str());
				unsigned group = M_MAX_UNSIGNED;

				// Try to recognize the use of the buffer from its name
				for(unsigned j = 0; j < MAX_SHADER_PARAMETER_GROUPS; ++j)
				{
					if(name.find(shaderParameterGroups[j]) != std::string::npos)
					{
						group = j;
						break;
					}
				}

				// If name is not recognized, search for a digit in the name and use that as the group index
				if(group == M_MAX_UNSIGNED)
				{
					for(unsigned j = 1; j < name.length(); ++j)
					{
						if(name[j] >= '0' && name[j] <= '5')
						{
							group = name[j] - '0';
							break;
						}
					}
				}

				if(group >= MAX_SHADER_PARAMETER_GROUPS)
				{
					YUMELOG_WARN("Skipping unrecognized uniform block " + name + " in shader program " + vertexShader_->GetFullName() +
						" " + pixelShader_->GetFullName());
					continue;
				}

				// Find total constant buffer data size
				int dataSize;
				glGetActiveUniformBlockiv(object_,blockIndex,GL_UNIFORM_BLOCK_DATA_SIZE,&dataSize);
				if(!dataSize)
					continue;

				unsigned bindingIndex = group;
				// Vertex shader constant buffer bindings occupy slots starting from zero to maximum supported, pixel shader bindings
				// from that point onward
				if(name.find("PS") != std::string::npos)
					bindingIndex += MAX_SHADER_PARAMETER_GROUPS;

				glUniformBlockBinding(object_,blockIndex,bindingIndex);
				blockToBinding[blockIndex] = bindingIndex;

				constantBuffers_[bindingIndex] = SharedPtr<YumeConstantBuffer>(rhi_->GetOrCreateConstantBuffer(bindingIndex,(unsigned)dataSize));
			}
		}
#endif

		// Check for shader parameters and texture units
		for(int i = 0; i < uniformCount; ++i)
		{
			unsigned type;
			int count;

			glGetActiveUniform(object_,(GLuint)i,MAX_PARAMETER_NAME_LENGTH,0,&count,&type,uniformName);
			int location = glGetUniformLocation(object_,uniformName);

			// Check for array index included in the name and strip it
			YumeString name(uniformName);
			unsigned index = name.find('[');
			if(index != 0xffffffff)
			{
				// If not the first index, skip
				if(name.find_first_of("[0]",index) == 0xffffffff)
					continue;

				name = name.substr(0,index);
			}

			if(name[0] == 'c')
			{
				// Store constant uniform
				YumeString paramName = name.substr(1);
				ShaderParameter newParam;
				newParam.type_ = type;
				newParam.location_ = location;

#ifndef GL_ES_VERSION_2_0
				// If running OpenGL 3, the uniform may be inside a constant buffer
				if(newParam.location_ < 0 && YumeGLRenderer::GetGL3Support())
				{
					int blockIndex,blockOffset;
					glGetActiveUniformsiv(object_,1,(const GLuint*)&i,GL_UNIFORM_BLOCK_INDEX,&blockIndex);
					glGetActiveUniformsiv(object_,1,(const GLuint*)&i,GL_UNIFORM_OFFSET,&blockOffset);
					if(blockIndex >= 0)
					{
						newParam.location_ = blockOffset;
						newParam.bufferPtr_ = constantBuffers_[blockToBinding[blockIndex]].get();
					}
				}
#endif

				if(newParam.location_ >= 0)
					shaderParameters_[GenerateHash(paramName)] = newParam;
			}
			else if(location >= 0 && name[0] == 's')
			{
				// Set the samplers here so that they do not have to be set later
				int unit = rhi_->GetTextureUnit(name.substr(1));
				if(unit >= MAX_TEXTURE_UNITS)
				{
					// If texture unit name is not recognized, search for a digit in the name and use that as the unit index
					for(unsigned j = 1; j < name.length(); ++j)
					{
						if(name[j] >= '0' && name[j] <= '9')
						{
							unit = name[j] - '0';
							break;
						}
					}
				}

				if(unit < MAX_TEXTURE_UNITS)
				{
					useTextureUnit_[unit] = true;
					glUniform1iv(location,1,&unit);
				}
			}
		}

		return true;
	}

	YumeShaderVariation* YumeGLShaderProgram::GetVertexShader() const
	{
		return vertexShader_;
	}

	YumeShaderVariation* YumeGLShaderProgram::GetPixelShader() const
	{
		return pixelShader_;
	}

	bool YumeGLShaderProgram::HasParameter(YumeHash param) const
	{
		return shaderParameters_.find(param) != shaderParameters_.end();
	}

	const ShaderParameter* YumeGLShaderProgram::GetParameter(YumeHash param) const
	{
		YumeMap<YumeHash,ShaderParameter>::const_iterator i = shaderParameters_.find(param);
		if(i != shaderParameters_.end())
			return &i->second;
		else
			return 0;
	}

	bool YumeGLShaderProgram::NeedParameterUpdate(ShaderParameterGroup group,const void* source)
	{
		// If global framenumber has changed, invalidate all per-program parameter sources now
		if(globalFrameNumber != frameNumber_)
		{
			for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
				parameterSources_[i] = (const void*)M_MAX_UNSIGNED;
			frameNumber_ = globalFrameNumber;
		}

		// The shader program may use a mixture of constant buffers and individual uniforms even in the same group
#ifndef GL_ES_VERSION_2_0
		bool useBuffer = constantBuffers_[group].get() || constantBuffers_[group + MAX_SHADER_PARAMETER_GROUPS].get();
		bool useIndividual = !constantBuffers_[group].get() || !constantBuffers_[group + MAX_SHADER_PARAMETER_GROUPS].get();
		bool needUpdate = false;

		if(useBuffer && globalParameterSources[group] != source)
		{
			globalParameterSources[group] = source;
			needUpdate = true;
		}

		if(useIndividual && parameterSources_[group] != source)
		{
			parameterSources_[group] = source;
			needUpdate = true;
		}

		return needUpdate;
#else
		if(parameterSources_[group] != source)
		{
			parameterSources_[group] = source;
			return true;
		}
		else
			return false;
#endif
	}

	void YumeGLShaderProgram::ClearParameterSource(ShaderParameterGroup group)
	{
		// The shader program may use a mixture of constant buffers and individual uniforms even in the same group
#ifndef GL_ES_VERSION_2_0
		bool useBuffer = constantBuffers_[group].get() || constantBuffers_[group + MAX_SHADER_PARAMETER_GROUPS].get();
		bool useIndividual = !constantBuffers_[group].get() || !constantBuffers_[group + MAX_SHADER_PARAMETER_GROUPS].get();

		if(useBuffer)
			globalParameterSources[group] = (const void*)M_MAX_UNSIGNED;
		if(useIndividual)
			parameterSources_[group] = (const void*)M_MAX_UNSIGNED;
#else
		parameterSources_[group] = (const void*)M_MAX_UNSIGNED;
#endif
	}

	void YumeGLShaderProgram::ClearParameterSources()
	{
		++globalFrameNumber;
		if(!globalFrameNumber)
			++globalFrameNumber;

#ifndef GL_ES_VERSION_2_0
		for(unsigned i = 0; i < MAX_SHADER_PARAMETER_GROUPS; ++i)
			globalParameterSources[i] = (const void*)M_MAX_UNSIGNED;
#endif
	}

	void YumeGLShaderProgram::ClearGlobalParameterSource(ShaderParameterGroup group)
	{
		globalParameterSources[group] = (const void*)M_MAX_UNSIGNED;
	}

}
