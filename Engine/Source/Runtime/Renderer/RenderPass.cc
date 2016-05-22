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
#include "RenderPass.h"
#include "RenderCall.h"
#include "YumeRHI.h"

#include "YumeResourceManager.h"
#include "Core/YumeXmlFile.h"
#include "Core/YumeFile.h"
#include "Core/YumeDefaults.h"

#include "Logging/logging.h"

#include <pugixml/src/pugixml.hpp>

namespace YumeEngine
{
	static const char* addressModeNames[] =
	{
		"Wrap",
		"Mirror",
		"Clamp",
		"Border",
		0
	};

	static const char* filterModeNames[] =
	{
		"Nearest",
		"Bilinear",
		"Trilinear",
		"Anisotropic",
		"default",
		0
	};

	static const char* comparisonNames[] =
	{
		"Never",
		"Equal",
		"NotEqual",
		"Less",
		"LessEqual",
		"Greater",
		"GreaterEqual",
		"Always"
	};

	RenderPass::RenderPass()
	{
	}

	RenderPass::~RenderPass()
	{

		RenderTargets::iterator It = renderTargets_.begin();

		for(It; It != renderTargets_.end(); ++It)
		{
			RenderTargetDesc desc = It->second->GetDesc();
			if(desc.Usage != TextureUsage::TEXTURE_UAV)
			{
				static_cast<Texture2DPtr>(It->second)->Release();
			}
			else
			{
				static_cast<Texture3DPtr>(It->second)->Release();
			}
		}
	}

	void RenderPass::AddTexture(unsigned index,const YumeString& callName,TexturePtr tex)
	{
		RenderCallPtr call = GetCallByName(callName);

		if(call)
			call->SetInput(index,tex);
	}

	void RenderPass::Load(const YumeString& resource,bool isPostProcess)
	{
		YumeXmlFile* fullPath = gYume->pResourceManager->PrepareResource<YumeXmlFile>(resource);


		YumeString xmlData = fullPath->GetXml();

		if(xmlData.length())
		{
			pugi::xml_document doc;
			doc.load(xmlData.c_str());

			typedef pugi::xml_node XmlNode;

			XmlNode Root = doc.child("Yume");
			XmlNode Rts = Root.child("RenderTargets");
			XmlNode RenderCalls = Root.child("RenderCalls");
			XmlNode Samplers = Root.child("Samplers");

			unsigned commandCount = 0;
			unsigned inputCount = 0;
			unsigned outputCount = 0;
			unsigned rtCount = 0;
			unsigned samplerCount = 0;


			for(XmlNode child = Rts.first_child(); child; child = child.next_sibling())
			{
				rtCount++;

				const char* type = child.name();

				const char* name = child.attribute("Name").as_string();
				const char* format = child.attribute("Format").as_string();
				const char* mips = child.attribute("Mips").as_string();
				const char* width = child.attribute("Width").as_string();
				const char* height = child.attribute("Height").as_string();
				const char* depth = child.attribute("Height").as_string();
				const char* arraySize = child.attribute("ArraySize").as_string();
				const char* clearColor= child.attribute("ClearColor").as_string();
				const char* stencil = child.attribute("Stencil").as_string();

				RenderTargetDesc desc;
				ZeroMemory(&desc,sizeof(desc));

				if(strlen(width) > 0)
					desc.Width = atoi(width);
				if(strlen(height) > 0)
					desc.Height = atoi(height);
				if(strlen(depth) > 0)
					desc.Depth = atoi(depth);

				const char* size = child.attribute("Size").as_string();

				if(strlen(size) > 0)
				{
					YumeVector<YumeString>::type sizeVector = ParseFlags(size);
					int ws = atoi(sizeVector[0].c_str());
					int hs = atoi(sizeVector[1].c_str());

					desc.Width = gYume->pRHI->GetWidth() / ws;
					desc.Height = gYume->pRHI->GetHeight() / hs;
				}

				desc.ArraySize = atoi(arraySize);
				desc.Format = gYume->pRHI->GetFormatNs(format);
				desc.Mips = atoi(mips);

				desc.ClearColor = YumeColor(0,0,0,0);
				desc.Name = name;

				if(strcmp(type,"Rt") == 0)
				{
					desc.Type = RT_INPUT | RT_OUTPUT;
					desc.Usage = TextureUsage::TEXTURE_RENDERTARGET;
				}

				if(strcmp(type,"Ds") == 0)
				{
					desc.Type = RT_DEPTHSTENCIL;

					if(strcmp(stencil,"readonly") == 0)
						desc.Usage = TextureUsage::TEXTURE_DEPTHSTENCIL_READONLY;
					else
						desc.Usage = TextureUsage::TEXTURE_DEPTHSTENCIL;
				}

				if(strcmp(type,"Uav") == 0)
				{
					desc.Type = RT_UAV;
					desc.Usage = TextureUsage::TEXTURE_UAV;
				}

				TexturePtr textureTarget = 0;
				if(desc.Usage == TextureUsage::TEXTURE_UAV)
				{
					textureTarget = gYume->pRHI->CreateTexture3D();
					textureTarget->SetName(desc.Name);
					static_cast<Texture3DPtr>(textureTarget)->SetSize(desc.Width,desc.Height,desc.Depth,desc.Format,desc.Usage);
				}
				else
				{
					textureTarget = gYume->pRHI->CreateTexture2D();
					textureTarget->SetName(desc.Name);
					static_cast<Texture2DPtr>(textureTarget)->SetSize(desc.Width,desc.Height,desc.Format,desc.Usage,desc.ArraySize,desc.Mips);
				}
				textureTarget->SetName(desc.Name);
				textureTarget->SetDesc(desc);

				renderTargets_.insert(MakePair(desc.Name,textureTarget));
			}

			//Samplers
			for(XmlNode child = Samplers.first_child(); child; child = child.next_sibling())
			{
				samplerCount++;

				const char* name = child.attribute("Name").as_string();
				const char* filter = child.attribute("Filter").as_string();
				const char* comparison = child.attribute("Comprasion").as_string();
				const char* addressu = child.attribute("AddressU").as_string();
				const char* addressv = child.attribute("AddressV").as_string();
				const char* addressw = child.attribute("AddressW").as_string();

				SamplerStateDesc desc;
				ZeroMemory(&desc,sizeof(desc));

				desc.Name = name;

				for(int i=0; i < 4; ++i)
				{
					if(strcmp(addressu,addressModeNames[i]) == 0)
						desc.AddressModeU = (TextureAddressModes)i;
					if(strcmp(addressv,addressModeNames[i]) == 0)
						desc.AddressModeV = (TextureAddressModes)i;
					if(strcmp(addressw,addressModeNames[i]) == 0)
						desc.AddressModeW = (TextureAddressModes)i;
				}

				for(int i=0; i < 5; ++i)
				{
					if(strcmp(filter,filterModeNames[i]) == 0)
						desc.Filter = (TextureFilterModes)i;
				}

				for(int i=0; i < 8; ++i)
					if(strcmp(comparison,comparisonNames[i]) == 0)
						desc.ComparisonFunc = (TextureComparisonFunctions)i;

				unsigned samplerId = gYume->pRHI->CreateSamplerState(desc);

				samplers_.insert(MakePair(YumeString(name),MakePair(desc,samplerId)));
			}

			//Calls

			for(XmlNode child = RenderCalls.first_child(); child; child = child.next_sibling())
			{
				commandCount++;

				const char* type = child.name();
				CallType ct;

				if(strcmp(type,"Clear") == 0)
				{
					ct = CallType::CLEAR;
				}
				else if(strcmp(type,"Scene") == 0)
				{
					ct = CallType::SCENE;
				}
				else if(strcmp(type,"InjectLPV") == 0)
				{
					ct = CallType::LPV_INJECT;
				}
				else if(strcmp(type,"NormalizeLPV") == 0)
				{
					ct = CallType::LPV_NORMALIZE;
				}
				else if(strcmp(type,"PropagateLPV") == 0)
				{
					ct = CallType::LPV_PROPAGATE;
				}
				else if(strcmp(type,"Mips") == 0)
				{
					ct = CallType::GENERATEMIPS;
				}
				else if(strcmp(type,"Triangle") == 0)
				{
					ct = CallType::FSTRIANGLE;
				}
				else if(strcmp(type,"SvoInject") == 0)
				{
					ct = CallType::SVO_INJECT;
				}
				else if(strcmp(type,"AdaptLuminance") == 0)
				{
					ct = CallType::ADAPT_LUMINANCE;
				}

				const char* passName = child.attribute("PassName").as_string();
				const char* identifier = child.attribute("Identifier").as_string();

				const char* vertexShader = child.attribute("Vs").as_string();
				const char* vertexEntry = child.attribute("VsEntry").as_string();

				const char* pixelShader = child.attribute("Ps").as_string();
				const char* pixelEntry = child.attribute("PsEntry").as_string();

				const char* geometryShader = child.attribute("Gs").as_string();
				const char* geometryEntry = child.attribute("GsEntry").as_string();

				const char* singleOutput = child.attribute("Output").as_string();

				const char* dstencil = child.attribute("Stencil").as_string();



				RenderCallPtr renderCall = YumeAPINew RenderCall(ct,vertexShader,pixelShader,geometryShader,vertexEntry,pixelEntry,geometryEntry);
				renderCall->SetIdentifier(identifier);
				renderCall->SetPassName(passName);
				renderCall->SetPostProcessPass(isPostProcess);

				if(strlen(singleOutput) > 0)
				{
					TexturePtr sOutput = GetTextureByName(singleOutput);
					renderCall->SetOutput(0,sOutput);

					if(strcmp(singleOutput,"Backbuffer") == 0)
						renderCall->SetBackbufferWrite(true);
				}

				if(strlen(dstencil) > 0)
				{
					renderCall->SetDepthStencil(GetTextureByName(dstencil));
				}



				XmlNode samplerBindings = child.child("Samplers");
				XmlNode clearTargets = child.child("Targets");
				XmlNode Outputs = child.child("Outputs");
				XmlNode Params = child.child("Parameters");
				XmlNode Inputs = child.child("Inputs");

				for(XmlNode param = Params.first_child(); param; param = param.next_sibling())
				{
					const char* name = param.attribute("Name").as_string();
					YumeString value = param.attribute("Value").as_string();

					if(value.Contains(' ')) //Its a vector
					{
						unsigned wsCount = CountElements(value.c_str(),' ');

						if(wsCount == 4) //4-component vector
						{
							DirectX::XMFLOAT4 v4 = ToVector4(value.c_str());
							renderCall->SetShaderParameter(name,v4);
						}

						if(wsCount == 3)
						{
							DirectX::XMFLOAT3 v3 = ToVector3(value.c_str());
							renderCall->SetShaderParameter(name,v3);
						}
					}
					else
					{
						if(!value.Compare("false"))
						{
							renderCall->SetShaderParameter(name,false);
						}
						else if(!value.Compare("true"))
						{
							renderCall->SetShaderParameter(name,true);
						}
						else
							renderCall->SetShaderParameter(name,(float)atof(value.c_str()));

					}
				}

				unsigned targetCount = 0;
				for(XmlNode clearTarget = clearTargets.first_child(); clearTarget; clearTarget = clearTarget.next_sibling())
				{
					const char* tType = clearTarget.name();

					const char* name = clearTarget.attribute("Name").as_string();
					const char* cColor = clearTarget.attribute("ClearColor").as_string();

					TexturePtr target = GetTextureByName(name);

					DirectX::XMFLOAT4 clearColorV = ToVector4(cColor);

					target->SetClearColor(YumeColor(clearColorV.x,clearColorV.y,clearColorV.z,clearColorV.w));

					if(strcmp(tType,"Rt") == 0)
						renderCall->SetInput(targetCount++,target);
					else
						renderCall->SetDepthStencil(target);
				}

				for(XmlNode s = samplerBindings.first_child(); s; s = s.next_sibling())
				{
					const char* samplerType = s.name();

					const char* samplerName = s.attribute("Name").as_string();
					const char* slot = s.attribute("Register").as_string();

					Pair<SamplerStateDesc,unsigned>& samplerId = GetSamplerByName(samplerName);
					samplerId.first.Register = atoi(slot);

					if(strcmp(samplerType,"Ps") == 0)
					{
						renderCall->SetSampler(PS,samplerId.first.Register,samplerId.second);
					}

					if(strcmp(samplerType,"Vs") == 0)
					{
						renderCall->SetSampler(VS,samplerId.first.Register,samplerId.second);
					}
				}

				const char* flags = child.attribute("Flags").as_string();

				YumeVector<YumeString>::type flagsVector = ParseFlags(flags);


				for(XmlNode output = Outputs.first_child(); output; output = output.next_sibling())
				{
					outputCount++;

					const char* index = output.attribute("Index").as_string();
					const char* name = output.attribute("Name").as_string();

					renderCall->SetOutput(atoi(index),GetTextureByName(name));
				}


				for(XmlNode input = Inputs.first_child(); input;input = input.next_sibling())
				{
					inputCount++;

					const char* index = input.attribute("Index").as_string();
					const char* name = input.attribute("Name").as_string();

					if(strcmp(name,"Backbuffer") == 0)
						renderCall->SetBackbufferRead(true);

					renderCall->SetInput(atoi(index),GetTextureByName(name));
				}

				if(flagsVector.size())
				{
					for(int i=0; i < flagsVector.size(); ++i)
					{
						if(strcmp(flagsVector[i].c_str(),"NOBLEND") == 0)
						{
							renderCall->SetMiscRenderingFlags(RF_NOBLENDSTATE);
						}
						if(strcmp(flagsVector[i].c_str(),"NODS") == 0)
						{
							renderCall->SetMiscRenderingFlags(RF_NODEPTHSTENCIL);
						}

						if(strcmp(flagsVector[i].c_str(),"CLEAR_DEPTH") == 0)
							renderCall->SetClearFlags(CLEAR_DEPTH);

						if(strcmp(flagsVector[i].c_str(),"CLEAR") == 0)
							renderCall->SetMiscRenderingFlags(RF_CLEAR);

						if(strcmp(flagsVector[i].c_str(),"CLEAR_COLOR") == 0)
							renderCall->SetClearFlags(renderCall->GetClearFlags() | CLEAR_COLOR);

						if(!strcmp(flagsVector[i].c_str(),"SHADOW"))
							renderCall->SetShadowPass(true);

						if(!strcmp(flagsVector[i].c_str(),"VOXELIZE"))
							renderCall->SetVoxelizePass(true);

						if(!strcmp(flagsVector[i].c_str(),"DEFERRED_LIGHTS"))
							renderCall->SetDeferredLightPass(true);

						if(!strcmp(flagsVector[i].c_str(),"SKYBOX"))
							renderCall->SetSkyboxPass(true);

						if(!strcmp(flagsVector[i].c_str(),"DEFERRED"))
							renderCall->SetDeferred(true);

						if(!strcmp(flagsVector[i].c_str(),"FORWARD"))
							renderCall->SetForwardPass(true);

						if(!strcmp(flagsVector[i].c_str(),"WRITESTENCIL"))
							renderCall->SetWriteStencil(true);
					}
				}
				AddRenderCall(renderCall);
			}

			YUMELOG_INFO("Render Calls " << resource.c_str() << " is loaded successfully." <<
				"Post Process " << isPostProcess <<
				"Render Target Count: " << rtCount <<
				"Input Count: " << inputCount <<
				"Output Count: " << outputCount <<
				"Sampler Count: " << samplerCount <<
				"Render Command Count: " << commandCount);
		}
		else
		{
			YUMELOG_ERROR("Render Call " << resource.c_str() << " couldn't be loaded. This should not happen! ");
		}
	}

	const Variant& RenderPass::GetShaderParameter(YumeHash param) const
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			if(calls_[i]->ContainsParameter(param))
			{
				const YumeMap<YumeHash,Variant>::const_iterator it= calls_[i]->GetShaderVariants().find(param);

				return it->second;
			}
		}
		return Variant();
	}

	YumeTexture* RenderPass::GetTextureByName(const YumeString& name)
	{
		RenderTargets::iterator It = renderTargets_.find(name);

		if(It != renderTargets_.end())
			return It->second;
		else
			return 0;
	}


	Pair<SamplerStateDesc,unsigned>& RenderPass::GetSamplerByName(const YumeString& name)
	{
		Samplers::iterator It = samplers_.find(name);

		if(It != samplers_.end())
			return It->second;
	}

	void RenderPass::AddRenderCall(RenderCall* call)
	{
		calls_.push_back(call);
	}

	void RenderPass::RemoveRenderCall(RenderCall* call)
	{
		calls_.Remove(call);
	}

	RenderCallPtr RenderPass::GetCallByName(const YumeString& name)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			RenderCallPtr c = calls_[i];

			if(!c->GetPassName().Compare(name))
				return c;
		}
		return 0;
	}

	void RenderPass::SetShaderParameter(YumeHash param,const Variant& value)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			if(calls_[i]->ContainsParameter(param))
			{
				calls_[i]->SetShaderParameter(param,value);
			}
		}
	}

	void RenderPass::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT3& vector)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			if(calls_[i]->ContainsParameter(param))
			{
				calls_[i]->SetShaderParameter(param,vector);
			}
		}
	}

	void RenderPass::DisableRenderCalls(const YumeString& id)
	{
		unsigned enabledCalls = 0;
		for(int i=0; i < calls_.size(); ++i)
		{
			RenderCallPtr c = calls_[i];

			if(!c->GetIdentifier().Compare(id))
				c->SetEnabled(false);
			else
				++enabledCalls;
		}

		//Find the last render call and make sure its outputting to backbuffer. This may break tho not sure. Of course it breaks something

		//unsigned last = enabledCalls - 1;

		//RenderCallPtr c = calls_[last];

		//c->SetOutput(0,0);
	}

	void RenderPass::EnableRenderCalls(const YumeString& id)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			RenderCallPtr c = calls_[i];

			if(!c->GetIdentifier().Compare(id))
				c->SetEnabled(true);
		}
	}


	void RenderPass::SetShaderParameter(YumeHash param,const DirectX::XMFLOAT4& vector)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			if(calls_[i]->ContainsParameter(param))
			{
				calls_[i]->SetShaderParameter(param,vector);
			}
		}
	}

	void RenderPass::SetShaderParameter(YumeHash param,const DirectX::XMMATRIX& matrix)
	{
		for(int i=0; i < calls_.size(); ++i)
		{
			if(!calls_[i])
				assert(false);

			if(calls_[i]->ContainsParameter(param))
			{
				calls_[i]->SetShaderParameter(param,matrix);
			}
		}
	}


}
