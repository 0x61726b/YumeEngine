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
#include "RenderCall.h"

#include "YumeShaderVariation.h"
#include "YumeRHI.h"

namespace YumeEngine
{

	RenderCall::RenderCall(CallType type,const YumeString& vs,const YumeString& ps,const YumeString& gs,const YumeString& vsEntry,
		const YumeString& psEntry,
		const YumeString& gsEntry):
		enabled_(true),
		shadowPass_(false),
		hasVsSampler_(false),
		hasPsSampler_(false),
		numVertexSamplers_(0),
		numPixelSamplers_(0),
		type_(type),
		addFlags_(0),
		vs_(0),
		gs_(0),
		ps_(0),
		passName(String::EMPTY),
		depthStencil_(0),
		numInputs_(0),
		numOutputs_(0)
	{
		vs_ = gYume->pRHI->GetShader(VS,vs,vsEntry,vsEntry);
		ps_ = gYume->pRHI->GetShader(PS,ps,psEntry,psEntry);
		gs_ = gYume->pRHI->GetShader(GS,gs,gsEntry,gsEntry);

		for(int i=0; i < MAX_TEXTURE_UNITS;++i)
		{
			vsSamplers_[i] = -1;
			psSamplers_[i] = -1;
		}

		clearColor = YumeColor(0,0,0,0);

		inputs_.resize(MAX_TEXTURE_UNITS);
		outputs_.resize(MAX_RENDERTARGETS);
	}

	RenderCall::~RenderCall()
	{
	}

	void RenderCall::SetSampler(ShaderType type,unsigned index,unsigned samplerId)
	{
		if(type == VS)
		{
			vsSamplers_[index] = samplerId;
			hasVsSampler_ = true;
			++numVertexSamplers_;
		}
		else if(type == PS)
		{
			psSamplers_[index] = samplerId;
			hasPsSampler_ = true;
			++numPixelSamplers_;
		}
	}

	void RenderCall::SetInput(unsigned index,Texture2DPtr target)
	{
		if(inputs_.size() <= index)
			inputs_.resize(index + 1);
		inputs_[index] = target;

		++numInputs_;
	}

	void RenderCall::SetOutput(unsigned index,Texture2DPtr target)
	{
		if(outputs_.size() <= index)
			outputs_.resize(index + 1);
		outputs_[index] = target;

		++numOutputs_;
	}

	void RenderCall::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT3& value)
	{
		shaderVectors3[param] = value;
	}

	void RenderCall::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT4& value)
	{
		shaderVectors4[param] = value;
	}

	void RenderCall::SetShaderParameter(YumeHash param,const DirectX::XMMATRIX& value)
	{
		shaderMatrices[param] = value;
	}

	void RenderCall::SetShaderParameter(YumeHash param,const Variant& value)
	{
		shaderVariants[param] = value;
	}

	bool RenderCall::ContainsParameter(YumeHash param)
	{
		return shaderVectors4.Contains(param) || shaderVectors3.Contains(param) || shaderMatrices.Contains(param) || shaderVariants.Contains(param);
	}

	void RenderCall::SetPassName(const YumeString& name)
	{
		passName = name;
	}

	void RenderCall::SetIdentifier(const YumeString& name)
	{
		identifier_ = name;
	}

	Texture2DPtr RenderCall::AddTexture(const RenderTargetDesc& desc)
	{
		Texture2DPtr renderTarget = gYume->pRHI->CreateTexture2D();
		renderTarget->SetName(desc.Name);
		renderTarget->SetSize(desc.Width,desc.Height,desc.Format,desc.Usage,desc.ArraySize,desc.Mips);
		renderTarget->SetDesc(desc);
		if(desc.Type & RenderTargetInOut::RT_INPUT)
			SetInput(desc.Index,renderTarget);
		if(desc.Type & RT_OUTPUT)
			SetOutput(desc.Index,renderTarget);
		if(desc.Type == RT_DEPTHSTENCIL)
			SetDepthStencil(renderTarget);

		return renderTarget;
	}

	void RenderCall::AddTextures(unsigned size,Texture2DPtr* textures)
	{
		for(int i=0; i < size; ++i)
		{
			Texture2DPtr t = *textures;
			const RenderTargetDesc& desc = t->GetDesc();

			if(t->GetDesc().Type & RT_INPUT)
				SetInput(desc.Index,t);
			if(t->GetDesc().Type & RT_OUTPUT)
				SetOutput(desc.Index,t);
			if(t->GetDesc().Type == RT_DEPTHSTENCIL)
				SetDepthStencil(t);

			textures++;
		}
	}

	void RenderCall::AddTexture(RenderTargetInOut type,unsigned index,Texture2DPtr tex)
	{
		if(type & RT_INPUT)
			SetInput(index,tex);
		if(type & RT_OUTPUT)
			SetOutput(index,tex);
	}

	void RenderCall::SetDepthStencil(Texture2DPtr target)
	{
		depthStencil_ = target;
	}
}
