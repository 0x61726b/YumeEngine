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
#ifndef __RenderCall_h__
#define __RenderCall_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum CallType
	{
		CLEAR,
		SCENE,
		FORWARDLIGHTS,
		LIGHTVOLUMES,
		GENERATEMIPS,
		FSTRIANGLE,
		LPV_INJECT,
		LPV_NORMALIZE,
		LPV_PROPAGATE,
		SVO_INJECT
	};

	class YumeAPIExport RenderCall : public YumeBase
	{
	public:
		RenderCall(CallType type,const YumeString& vs = "",const YumeString& ps = "",const YumeString& gs = "",const YumeString& vsEntry = "VS",
			const YumeString& psEntry = "PS",
			const YumeString& gsEntry = "GS");
		virtual ~RenderCall();

		void SetShaderParameter(YumeHash param,const Variant& value);
		void SetShaderParameter(YumeHash param,const DirectX::XMFLOAT3& value);
		void SetShaderParameter(YumeHash param,const DirectX::XMFLOAT4& value);
		void SetShaderParameter(YumeHash param,const DirectX::XMMATRIX& value);

		void SetInput(unsigned,TexturePtr target);
		void SetOutput(unsigned,TexturePtr target);
		void SetDepthStencil(TexturePtr target);
		bool ContainsParameter(YumeHash param);
		void SetSampler(ShaderType type,unsigned index,unsigned samplerId);
		void SetPassName(const YumeString& name);
		void SetIdentifier(const YumeString& name);

		


		void AddTexture(RenderTargetInOut type,unsigned index,TexturePtr tex);
		TexturePtr AddTexture(const RenderTargetDesc& desc);
		void AddTextures(unsigned size,TexturePtr* textures);

		unsigned GetRendererFlags() const { return addFlags_;}
		void SetMiscRenderingFlags(unsigned f) { addFlags_ = f; }

		CallType GetType() const { return type_; }

		YumeShaderVariation* GetVs() const { return vs_; }
		YumeShaderVariation* GetPs() const { return ps_; }
		YumeShaderVariation* GetGs() const { return gs_; }

		const YumeMap<YumeHash,Variant>::type& GetShaderVariants() { return shaderVariants; }
		const YumeMap<YumeHash,DirectX::XMFLOAT3>::type& GetShaderVectors3() { return shaderVectors3; }
		const YumeMap<YumeHash,DirectX::XMFLOAT4>::type& GetShaderVectors4() { return shaderVectors4; }
		const YumeMap<YumeHash,DirectX::XMMATRIX>::type& GetShaderMatrices() { return shaderMatrices; }

		const YumeString& GetPassName() const { return passName; }
		const YumeString& GetIdentifier() const { return identifier_; }

		unsigned GetVertexSampler(unsigned i) const { return vsSamplers_[i]; }
		unsigned GetPixelSampler(unsigned i) const { return psSamplers_[i]; }

		const YumeColor& GetClearColor(unsigned index) const { return inputs_[index]->GetDesc().ClearColor; }
		
		TexturePtr GetInput(unsigned index) const { return inputs_[index]; }

		TexturePtr GetOutput(unsigned index) const { return outputs_[index]; }

		unsigned GetNumVertexSamplers() const {return numVertexSamplers_;}
		unsigned GetNumPixelSamplers() const {return numPixelSamplers_;}

		void RemoveInput(int);

		unsigned GetNumInputs() const { return numInputs_; }
		unsigned GetNumOutputs() const { return numOutputs_; }

		bool HasVertexSampler() const { return hasVsSampler_; }
		bool HasPixelSampler() const { return hasPsSampler_; }
		bool IsShadowPass() const { return shadowPass_; }
		bool IsVoxelizePass() const { return voxelizePass_; }

		void SetClearFlags(unsigned flags ) { clearFlags = flags; }
		unsigned GetClearFlags() const { return clearFlags; }

		void SetShadowPass(bool b) { shadowPass_ = b; }
		void SetVoxelizePass(bool b) { voxelizePass_ = b; }

		void SetEnabled(bool enabled) { enabled_ = enabled; }
		bool GetEnabled() const { return enabled_; }

		bool GetDeferred() const { return deferred_; }
		void SetDeferred(bool enabled) { deferred_ = enabled; }

		Texture2DPtr GetDepthStencil() const { return depthStencil_; }
	private:
		bool enabled_;
		bool shadowPass_;
		bool voxelizePass_;
		
		bool hasVsSampler_;
		bool hasPsSampler_;

		unsigned numVertexSamplers_;
		unsigned numPixelSamplers_;

		unsigned numInputs_;
		unsigned numOutputs_;

		bool deferred_;

		YumeString passName;
		YumeString identifier_;

		CallType type_;
		YumeMap<YumeHash,Variant>::type shaderVariants;
		YumeMap<YumeHash,DirectX::XMFLOAT3>::type shaderVectors3;
		YumeMap<YumeHash,DirectX::XMFLOAT4>::type shaderVectors4;
		YumeMap<YumeHash,DirectX::XMMATRIX>::type shaderMatrices;
		unsigned clearFlags;
		unsigned addFlags_;

		YumeColor clearColor;

		int vsSamplers_[MAX_TEXTURE_UNITS];
		int psSamplers_[MAX_TEXTURE_UNITS];

		RenderTargetDesc rtDescs_[32];

		typedef YumeVector<SharedPtr<YumeTexture> > InputOutputRTs ;

		InputOutputRTs::type inputs_;
		InputOutputRTs::type outputs_;

		Texture2DPtr depthStencil_;

		YumeShaderVariation* vs_;
		YumeShaderVariation* ps_;
		YumeShaderVariation* gs_;
	};

	typedef RenderCall* RenderCallPtr;
}


//----------------------------------------------------------------------------
#endif
