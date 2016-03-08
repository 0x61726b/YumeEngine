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
#include "YumeRenderPass.h"
#include "YumeRenderer.h"
#include "YumeRHI.h"

#include "YumeShaderVariation.h"
#include "Logging/logging.h"

#include "Core/YumeFile.h"
#include "Core/YumeDefaults.h"
#include "Core/YumeXmlParser.h"

#include <boost/algorithm/string/case_conv.hpp>

namespace YumeEngine
{

	const char* blendModeNames[] =
	{
		"replace",
		"add",
		"multiply",
		"alpha",
		"addalpha",
		"premulalpha",
		"invdestalpha",
		"subtract",
		"subtractalpha",
		0
	};

	static const char* compareModeNames[] =
	{
		"always",
		"equal",
		"notequal",
		"less",
		"lessequal",
		"greater",
		"greaterequal",
		0
	};

	static const char* lightingModeNames[] =
	{
		"unlit",
		"pervertex",
		"perpixel",
		0
	};

	YumeRenderPass::YumeRenderPass(const YumeString& name):
		blendMode_(BLEND_REPLACE),
		depthTestMode_(CMP_LESSEQUAL),
		lightingMode_(LIGHTING_UNLIT),
		shadersLoadedFrameNumber_(0),
		depthWrite_(true),
		alphaMask_(false),
		isDesktop_(false)
	{
		name_ = name;
		boost::to_lower(name_);

		index_ = YumeRenderTechnique::GetPassIndex(name_);

		// Guess default lighting mode from pass name
		if(index_ == YumeRenderTechnique::basePassIndex || index_ == YumeRenderTechnique::alphaPassIndex || index_ == YumeRenderTechnique::materialPassIndex ||
			index_ == YumeRenderTechnique::deferredPassIndex)
			lightingMode_ = LIGHTING_PERVERTEX;
		else if(index_ == YumeRenderTechnique::lightPassIndex || index_ == YumeRenderTechnique::litBasePassIndex || index_ == YumeRenderTechnique::litAlphaPassIndex)
			lightingMode_ = LIGHTING_PERPIXEL;
	}

	YumeRenderPass::~YumeRenderPass()
	{
	}

	void YumeRenderPass::SetBlendMode(BlendMode mode)
	{
		blendMode_ = mode;
	}

	void YumeRenderPass::SetDepthTestMode(CompareMode mode)
	{
		depthTestMode_ = mode;
	}

	void YumeRenderPass::SetLightingMode(PassLightingMode mode)
	{
		lightingMode_ = mode;
	}

	void YumeRenderPass::SetDepthWrite(bool enable)
	{
		depthWrite_ = enable;
	}

	void YumeRenderPass::SetAlphaMask(bool enable)
	{
		alphaMask_ = enable;
	}

	void YumeRenderPass::SetIsDesktop(bool enable)
	{
		isDesktop_ = enable;
	}

	void YumeRenderPass::SetVertexShader(const YumeString& name)
	{
		vertexShaderName_ = name;
		ReleaseShaders();
	}

	void YumeRenderPass::SetPixelShader(const YumeString& name)
	{
		pixelShaderName_ = name;
		ReleaseShaders();
	}

	void YumeRenderPass::SetVertexShaderDefines(const YumeString& defines)
	{
		vertexShaderDefines_ = defines;
		ReleaseShaders();
	}

	void YumeRenderPass::SetPixelShaderDefines(const YumeString& defines)
	{
		pixelShaderDefines_ = defines;
		ReleaseShaders();
	}

	void YumeRenderPass::ReleaseShaders()
	{
		vertexShaders_.clear();
		pixelShaders_.clear();
	}

	void YumeRenderPass::MarkShadersLoaded(unsigned frameNumber)
	{
		shadersLoadedFrameNumber_ = frameNumber;
	}

	unsigned YumeRenderTechnique::basePassIndex = 0;
	unsigned YumeRenderTechnique::alphaPassIndex = 0;
	unsigned YumeRenderTechnique::materialPassIndex = 0;
	unsigned YumeRenderTechnique::deferredPassIndex = 0;
	unsigned YumeRenderTechnique::lightPassIndex = 0;
	unsigned YumeRenderTechnique::litBasePassIndex = 0;
	unsigned YumeRenderTechnique::litAlphaPassIndex = 0;
	unsigned YumeRenderTechnique::shadowPassIndex = 0;
	YumeHash YumeRenderTechnique::techHash_ = ("RenderTechnique");
	YumeMap<YumeString,unsigned>::type YumeRenderTechnique::passIndices;

	YumeRenderTechnique::YumeRenderTechnique():
		isDesktop_(true)
	{
		desktopSupport_ = true;
	}

	YumeRenderTechnique::~YumeRenderTechnique()
	{
	}

	YumeHash YumeRenderTechnique::GetType()
	{
		return techHash_;
	}

	bool YumeRenderTechnique::BeginLoad(YumeFile& source)
	{
		passes_.clear();

		SetMemoryUsage(sizeof(YumeRenderTechnique));

		YumeString xmlData = source.ReadString();

		pugi::xml_document doc;
		doc.load(xmlData.c_str());


		XmlNode root = doc.child("Yume");
		XmlNode technique = root.child("Technique");

		YumeString globalVS = technique.attribute("vs").as_string();
		YumeString globalPS = technique.attribute("ps").as_string();
		YumeString globalVSDefines = technique.attribute("vsdefines").as_string();
		YumeString globalPSDefines = technique.attribute("psdefines").as_string();

		if(!globalVSDefines.empty())
			globalVSDefines += ' ';
		if(!globalPSDefines.empty())
			globalPSDefines += ' ';

		bool globalAlphaMask = technique.attribute("alphamask").as_bool();

		for(XmlNode pass = technique.first_child(); pass; pass = pass.next_sibling())
		{
			YumeRenderPass* newPass = CreatePass(pass.attribute("name").as_string());

			if(!pass.attribute("vs").empty())
			{
				newPass->SetVertexShader(pass.attribute("vs").as_string());
				newPass->SetVertexShaderDefines(pass.attribute("vsdefines").as_string());
			}
			else
			{
				newPass->SetVertexShader(globalVS);
				newPass->SetVertexShaderDefines(globalVSDefines + pass.attribute("vsdefines").as_string());
			}
			if(!pass.attribute("ps").empty())
			{
				newPass->SetPixelShader(pass.attribute("ps").as_string());
				newPass->SetPixelShaderDefines(pass.attribute("psdefines").as_string());
			}
			else
			{
				newPass->SetPixelShader(globalPS);
				newPass->SetPixelShaderDefines(globalPSDefines + pass.attribute("psdefines").as_string());
			}

			if(!pass.attribute("lighting").empty())
			{
				YumeString lighting = pass.attribute("lighting").as_string();
				boost::to_lower(lighting);
				newPass->SetLightingMode((PassLightingMode)GetStringListIndex(lighting.c_str(),lightingModeNames,
					LIGHTING_UNLIT));
			}

			if(!pass.attribute("blend").empty())
			{
				YumeString blend = pass.attribute("blend").as_string();
				boost::to_lower(blend);
				newPass->SetBlendMode((BlendMode)GetStringListIndex(blend.c_str(),blendModeNames,BLEND_REPLACE));
			}

			if(!pass.attribute("depthtest").empty())
			{
				YumeString depthTest = pass.attribute("depthtest").as_string();
				boost::to_lower(depthTest);
				if(depthTest == "false")
					newPass->SetDepthTestMode(CMP_ALWAYS);
				else
					newPass->SetDepthTestMode((CompareMode)GetStringListIndex(depthTest.c_str(),compareModeNames,CMP_LESS));
			}

			if(!pass.attribute("depthwrite").empty())
				newPass->SetDepthWrite(pass.attribute("depthwrite").as_bool());

			if(!pass.attribute("alphamask").empty())
				newPass->SetAlphaMask(pass.attribute("alphamask").as_bool());
			else
				newPass->SetAlphaMask(globalAlphaMask);
		}

		return true;
	}

	void YumeRenderTechnique::SetIsDesktop(bool enable)
	{
		isDesktop_ = enable;
	}

	void YumeRenderTechnique::ReleaseShaders()
	{
		for(YumeVector<SharedPtr<YumeRenderPass> >::const_iterator i = passes_.begin(); i != passes_.end(); ++i)
		{
			YumeRenderPass* pass = i->get();
			if(pass)
				pass->ReleaseShaders();
		}
	}

	YumeRenderPass* YumeRenderTechnique::CreatePass(const YumeString& name)
	{
		YumeRenderPass* oldPass = GetPass(name);
		if(oldPass)
			return oldPass;

		SharedPtr<YumeRenderPass> newPass(new YumeRenderPass(name));
		unsigned passIndex = newPass->GetIndex();
		if(passIndex >= passes_.size())
			passes_.resize(passIndex + 1);
		passes_[passIndex] = newPass;

		// Calculate memory use now
		SetMemoryUsage((unsigned)(sizeof(YumeRenderTechnique) + GetNumPasses() * sizeof(YumeRenderPass)));

		return newPass.get();
	}

	void YumeRenderTechnique::RemovePass(const YumeString& name)
	{
		YumeString lower = name;
		boost::to_lower(lower);
		YumeMap<YumeString,unsigned>::const_iterator i = passIndices.find(lower);
		if(i == passIndices.end())
			return;
		else if(i->second < passes_.size() && passes_[i->second].get())
		{
			passes_[i->second].reset();
			SetMemoryUsage((unsigned)(sizeof(YumeRenderTechnique) + GetNumPasses() * sizeof(YumeRenderPass)));
		}
	}

	bool YumeRenderTechnique::HasPass(const YumeString& name) const
	{
		YumeString lower = name;
		boost::to_lower(lower);
		YumeMap<YumeString,unsigned>::const_iterator i = passIndices.find(lower);
		return i != passIndices.end() ? HasPass(i->second) : false;
	}

	YumeRenderPass* YumeRenderTechnique::GetPass(const YumeString& name) const
	{
		YumeString lower = name;
		boost::to_lower(lower);
		YumeMap<YumeString,unsigned>::const_iterator i = passIndices.find(lower);
		return i != passIndices.end() ? GetPass(i->second) : 0;
	}

	YumeRenderPass* YumeRenderTechnique::GetSupportedPass(const YumeString& name) const
	{
		YumeString lower = name;
		boost::to_lower(lower);
		YumeMap<YumeString,unsigned>::const_iterator i = passIndices.find(lower);
		return i != passIndices.end() ? GetSupportedPass(i->second) : 0;
	}

	unsigned YumeRenderTechnique::GetNumPasses() const
	{
		unsigned ret = 0;

		for(YumeVector<SharedPtr<YumeRenderPass> >::const_iterator i = passes_.begin(); i != passes_.end(); ++i)
		{
			if(i->get())
				++ret;
		}

		return ret;
	}

	YumeVector<YumeString>::type YumeRenderTechnique::GetPassNames() const
	{
		YumeVector<YumeString>::type ret;

		for(YumeVector<SharedPtr<YumeRenderPass> >::const_iterator i = passes_.begin(); i != passes_.end(); ++i)
		{
			YumeRenderPass* pass = i->get();
			if(pass)
				ret.push_back(pass->GetName());
		}

		return ret;
	}

	YumeVector<YumeRenderPass*>::type YumeRenderTechnique::GetPasses() const
	{
		YumeVector<YumeRenderPass*>::type ret;

		for(YumeVector<SharedPtr<YumeRenderPass> >::const_iterator i = passes_.begin(); i != passes_.end(); ++i)
		{
			YumeRenderPass* pass = i->get();
			if(pass)
				ret.push_back(pass);
		}

		return ret;
	}

	unsigned YumeRenderTechnique::GetPassIndex(const YumeString& passName)
	{
		// Initialize built-in pass indices on first call
		if(passIndices.empty())
		{
			basePassIndex = passIndices["base"] = 0;
			alphaPassIndex = passIndices["alpha"] = 1;
			materialPassIndex = passIndices["material"] = 2;
			deferredPassIndex = passIndices["deferred"] = 3;
			lightPassIndex = passIndices["light"] = 4;
			litBasePassIndex = passIndices["litbase"] = 5;
			litAlphaPassIndex = passIndices["litalpha"] = 6;
			shadowPassIndex = passIndices["shadow"] = 7;
		}

		YumeString nameLower = passName;
		boost::to_lower(nameLower);

		YumeMap<YumeString,unsigned>::iterator i = passIndices.find(nameLower);
		if(i != passIndices.end())
			return i->second;
		else
		{
			unsigned newPassIndex = passIndices.size();
			passIndices[nameLower] = newPassIndex;
			return newPassIndex;
		}
	}
}
