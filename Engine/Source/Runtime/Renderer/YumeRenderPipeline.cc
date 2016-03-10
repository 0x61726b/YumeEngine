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
#include "YumeRenderPipeline.h"
#include "Core/YumeXmlFile.h"
#include "YumeRHI.h"
#include "YumeMaterial.h"

#include "Engine/YumeEngine.h"

#include "Core/YumeDefaults.h"

#include <boost/algorithm/string.hpp>


namespace YumeEngine
{

	static const char* commandTypeNames[] =
	{
		"none",
		"clear",
		"scenepass",
		"quad",
		"forwardlights",
		"lightvolumes",
		"renderui",
		0
	};

	static const char* sortModeNames[] =
	{
		"fronttoback",
		"backtofront",
		0
	};

	extern const char* blendModeNames[];

	TextureUnit ParseTextureUnitName(YumeString name);

	void RenderTargetInfo::Load(const XmlNode& element)
	{
		name_ = element.attribute("name").as_string();
		tag_ = element.attribute("tag").as_string();
		enabled_ = element.attribute("enabled").as_bool();
		cubemap_ = element.attribute("cubemap").as_bool();

		YumeString formatName = element.attribute("format").as_string();

		YumeRHI* rhi_ = YumeEngine3D::Get()->GetRenderer();
		format_ = rhi_->GetFormatNs(formatName);

		filtered_ = element.attribute("filter").as_bool();
		sRGB_  = element.attribute("srgb").as_bool();
		persistent_ = element.attribute("persistent").as_bool();

		size_ = ToVector2(element.attribute("size").as_string());

		if(!element.attribute("sizedivisor").empty())
		{
			size_ = ToVector2(element.attribute("sizedivisor").as_string());
			sizeMode_ = SIZE_VIEWPORTDIVISOR;
		}
		else if(!element.attribute("sizemultiplier").empty())
		{
			size_ = ToVector2(element.attribute("sizemultiplier").as_string());
			sizeMode_ = SIZE_VIEWPORTMULTIPLIER;
		}

		if(!element.attribute("width").empty())
			size_.x_ = element.attribute("width").as_float();
		if(!element.attribute("height").empty())
			size_.y_ = element.attribute("height").as_float();
	}

	void RenderCommand::Load(const XmlNode& element)
	{
		YumeString t = element.attribute("type").as_string();
		type_ = (RenderCommandType)GetStringListIndex(element.attribute("type").as_string(),commandTypeNames,CMD_NONE);
		tag_ = element.attribute("tag").as_string();
		if(!element.attribute("enabled").empty())
			enabled_ = element.attribute("enabled").as_bool();
		if(!element.attribute("metadata").empty())
			metadata_ = element.attribute("metadata").as_string();

		switch(type_)
		{
		case CMD_CLEAR:
			if(!element.attribute("color").empty())
			{
				clearFlags_ |= CLEAR_COLOR;
				if(strcmp(element.attribute("color").as_string(),"fog") == 0)
					useFogColor_ = true;
				else
					clearColor_ = ToColor(element.attribute("color").as_string());
			}
			if(!element.attribute("depth").empty())
			{
				clearFlags_ |= CLEAR_DEPTH;
				clearDepth_ = element.attribute("depth").as_float();
			}
			if(!element.attribute("stencil").empty())
			{
				clearFlags_ |= CLEAR_STENCIL;
				clearStencil_ = (unsigned)element.attribute("stencil").as_int();
			}
			break;

		case CMD_SCENEPASS:
			pass_ = element.attribute("pass").as_string();
			sortMode_ =
				(RenderCommandSortMode)GetStringListIndex(element.attribute("sort").as_string(),sortModeNames,SORT_FRONTTOBACK);
			if(!element.attribute("marktostencil").empty())
				markToStencil_ = element.attribute("marktostencil").as_bool();
			if(!element.attribute("vertexlights").empty())
				vertexLights_ = element.attribute("vertexlights").as_bool();
			break;

		case CMD_FORWARDLIGHTS:
			pass_ = element.attribute("pass").as_string();
			if(!element.attribute("uselitbase").empty())
				useLitBase_ = element.attribute("uselitbase").as_bool();
			break;

		case CMD_LIGHTVOLUMES:
		case CMD_QUAD:
			vertexShaderName_ = element.attribute("vs").as_string();
			pixelShaderName_ = element.attribute("ps").as_string();
			vertexShaderDefines_ = element.attribute("vsdefines").as_string();
			pixelShaderDefines_ = element.attribute("psdefines").as_string();

			if(type_ == CMD_QUAD)
			{
				if(!element.attribute("blend").empty())
				{
					YumeString blend = element.attribute("blend").as_string();
					blendMode_ = ((BlendMode)GetStringListIndex(blend.c_str(),blendModeNames,BLEND_REPLACE));
				}

				XmlNode parameterElem = element.child("parameter");
				for(XmlNode parameterChild = parameterElem.first_child(); parameterChild; parameterChild = parameterChild.next_sibling())
				{
					YumeString name = parameterElem.attribute("name").as_string();
					shaderParameters_[name] = YumeMaterial::ParseShaderParameterValue(parameterChild.attribute("value").as_string());
				}
			}
			break;

		default:
			break;
		}

		// By default use 1 output, which is the viewport
		outputs_.resize(1);
		outputs_[0] = std::make_pair(YumeString("viewport"),FACE_POSITIVE_X);
		if(!element.attribute("output").empty())
			outputs_[0].first = element.attribute("output").as_string();
		if(!element.attribute("face").empty())
			outputs_[0].second = (CubeMapFace)element.attribute("face").as_int();
		if(!element.attribute("depthstencil").empty())
			depthStencilName_ = element.attribute("depthstencil").as_string();
		// Check for defining multiple outputs
		XmlNode outputElem = element.child("output");
		for(XmlNode oChild = outputElem.first_child(); oChild; oChild = oChild.next_sibling())
		{
			unsigned index = (unsigned)oChild.attribute("index").as_int();
			if(index < MAX_RENDERTARGETS)
			{
				if(index >= outputs_.size())
					outputs_.resize(index + 1);
				outputs_[index].first = oChild.attribute("name").as_string();
				outputs_[index].second = oChild.attribute("face").empty() ? (CubeMapFace)oChild.attribute("face").as_int() : FACE_POSITIVE_X;
			}
		}

		XmlNode textureElem = element.child("texture");
		if(!textureElem.empty())
		for(XmlNode tChild = element.first_child(); tChild; tChild = tChild.next_sibling())
		{
			TextureUnit unit = TU_DIFFUSE;
			if(!tChild.attribute("unit").empty())
				unit = ParseTextureUnitName(tChild.attribute("unit").as_string());
			if(unit < MAX_TEXTURE_UNITS)
			{
				YumeString name = tChild.attribute("name").as_string();
				textureNames_[unit] = name;
			}
		}
	}

	void RenderCommand::SetTextureName(TextureUnit unit,const YumeString& name)
	{
		if(unit < MAX_TEXTURE_UNITS)
			textureNames_[unit] = name;
	}

	void RenderCommand::SetShaderParameter(const YumeString& name,const Variant& value)
	{
		shaderParameters_[name] = value;
	}

	void RenderCommand::RemoveShaderParameter(const YumeString& name)
	{
		shaderParameters_.erase(name);
	}

	void RenderCommand::SetNumOutputs(unsigned num)
	{
		num = (unsigned)Clamp((int)num,1,MAX_RENDERTARGETS);
		outputs_.resize(num);
	}

	void RenderCommand::SetOutput(unsigned index,const YumeString& name,CubeMapFace face)
	{
		if(index < outputs_.size())
			outputs_[index] = std::make_pair(name,face);
		else if(index == outputs_.size() && index < MAX_RENDERTARGETS)
			outputs_.push_back(std::make_pair(name,face));
	}

	void RenderCommand::SetOutputName(unsigned index,const YumeString& name)
	{
		if(index < outputs_.size())
			outputs_[index].first = name;
		else if(index == outputs_.size() && index < MAX_RENDERTARGETS)
			outputs_.push_back(std::make_pair(name,FACE_POSITIVE_X));
	}

	void RenderCommand::SetOutputFace(unsigned index,CubeMapFace face)
	{
		if(index < outputs_.size())
			outputs_[index].second = face;
		else if(index == outputs_.size() && index < MAX_RENDERTARGETS)
			outputs_.push_back(std::make_pair(EmptyString,face));
	}


	void RenderCommand::SetDepthStencilName(const YumeString& name)
	{
		depthStencilName_ = name;
	}

	const YumeString& RenderCommand::GetTextureName(TextureUnit unit) const
	{
		return unit < MAX_TEXTURE_UNITS ? textureNames_[unit] : EmptyString;
	}

	const Variant& RenderCommand::GetShaderParameter(const YumeString& name) const
	{
		YumeMap<YumeHash,Variant>::const_iterator i = shaderParameters_.find(name);
		return i != shaderParameters_.end() ? i->second : Variant::EMPTY;
	}

	const YumeString& RenderCommand::GetOutputName(unsigned index) const
	{
		return index < outputs_.size() ? outputs_[index].first : EmptyString;
	}

	CubeMapFace RenderCommand::GetOutputFace(unsigned index) const
	{
		return index < outputs_.size() ? outputs_[index].second : FACE_POSITIVE_X;
	}

	YumeRenderPipeline::YumeRenderPipeline()
	{
	}

	YumeRenderPipeline::~YumeRenderPipeline()
	{
	}

	SharedPtr<YumeRenderPipeline> YumeRenderPipeline::Clone()
	{
		SharedPtr<YumeRenderPipeline> newRenderPath(new YumeRenderPipeline());
		newRenderPath->renderTargets_ = renderTargets_;
		newRenderPath->commands_ = commands_;
		return newRenderPath;
	}

	bool YumeRenderPipeline::Load(YumeXmlFile* file)
	{
		renderTargets_.clear();
		commands_.clear();

		return Append(file);
	}

	bool YumeRenderPipeline::Append(YumeXmlFile* file)
	{
		if(!file)
			return false;

		YumeString xmlData = file->GetXml();

		pugi::xml_document doc;
		doc.load(xmlData.c_str());

		XmlNode rootElem = doc.child("Pipeline");
		if(!rootElem)
			return false;

		XmlNode rt = rootElem.child("rendertarget");
		for(XmlNode rtChild = rt.first_child(); rtChild; rtChild = rtChild.next_sibling())
		{
			RenderTargetInfo info;
			info.Load(rtChild);
			
			boost::trim(info.name_);
			if(!info.name_.empty())
				renderTargets_.push_back(info);
		}

		XmlNode cmd = rootElem.child("Commands");
		for(XmlNode cmdChild = cmd.first_child(); cmdChild; cmdChild = cmdChild.next_sibling())
		{
			RenderCommand cmd;
			cmd.Load(cmdChild);
			if(cmd.type_ != CMD_NONE)
				commands_.push_back(cmd);
		}

		return true;
	}

	void YumeRenderPipeline::SetEnabled(const YumeString& tag,bool active)
	{
		for(unsigned i = 0; i < renderTargets_.size(); ++i)
		{
			if(strcmp(renderTargets_[i].tag_.c_str(),tag.c_str()) != 0)
				renderTargets_[i].enabled_ = active;
		}

		for(unsigned i = 0; i < commands_.size(); ++i)
		{
			if(strcmp(commands_[i].tag_.c_str(),tag.c_str()) != 0)
				commands_[i].enabled_ = active;
		}
	}

	void YumeRenderPipeline::ToggleEnabled(const YumeString& tag)
	{
		for(unsigned i = 0; i < renderTargets_.size(); ++i)
		{
			if(strcmp(renderTargets_[i].tag_.c_str(),tag.c_str()) != 0)
				renderTargets_[i].enabled_ = !renderTargets_[i].enabled_;
		}

		for(unsigned i = 0; i < commands_.size(); ++i)
		{
			if(strcmp(commands_[i].tag_.c_str(),tag.c_str()) != 0)
				commands_[i].enabled_ = !commands_[i].enabled_;
		}
	}

	void YumeRenderPipeline::SetRenderTarget(unsigned index,const RenderTargetInfo& info)
	{
		if(index < renderTargets_.size())
			renderTargets_[index] = info;
		else if(index == renderTargets_.size())
			AddRenderTarget(info);
	}

	void YumeRenderPipeline::AddRenderTarget(const RenderTargetInfo& info)
	{
		renderTargets_.push_back(info);
	}

	void YumeRenderPipeline::RemoveRenderTarget(unsigned index)
	{
		renderTargets_.erase(renderTargets_.begin()+index);
	}

	void YumeRenderPipeline::RemoveRenderTarget(const YumeString& name)
	{
		for(unsigned i = 0; i < renderTargets_.size(); ++i)
		{
			if(strcmp(renderTargets_[i].name_.c_str(),name.c_str()) != 0)
			{
				renderTargets_.erase(renderTargets_.begin() + i);
				return;
			}
		}
	}

	void YumeRenderPipeline::RemoveRenderTargets(const YumeString& tag)
	{
		for(unsigned i = renderTargets_.size() - 1; i < renderTargets_.size(); --i)
		{
			if(strcmp(renderTargets_[i].tag_.c_str(),tag.c_str()) != 0)
				renderTargets_.erase(renderTargets_.begin() + i);
		}
	}

	void YumeRenderPipeline::SetCommand(unsigned index,const RenderCommand& command)
	{
		if(index < commands_.size())
			commands_[index] = command;
		else if(index == commands_.size())
			AddCommand(command);
	}

	void YumeRenderPipeline::AddCommand(const RenderCommand& command)
	{
		commands_.push_back(command);
	}

	void YumeRenderPipeline::InsertCommand(unsigned index,const RenderCommand& command)
	{
		commands_.insert(commands_.begin() + index,command);
	}

	void YumeRenderPipeline::RemoveCommand(unsigned index)
	{
		commands_.erase(commands_.begin() + index);
	}

	void YumeRenderPipeline::RemoveCommands(const YumeString& tag)
	{
		for(unsigned i = commands_.size() - 1; i < commands_.size(); --i)
		{
			if(strcmp(commands_[i].tag_.c_str(),tag.c_str()) != 0)
				commands_.erase(commands_.begin() + i);
		}
	}

	void YumeRenderPipeline::SetShaderParameter(const YumeString& name,const Variant& value)
	{
		YumeHash nameHash(name);

		for(unsigned i = 0; i < commands_.size(); ++i)
		{
			YumeMap<YumeHash,Variant>::iterator j = commands_[i].shaderParameters_.find(nameHash);
			if(j != commands_[i].shaderParameters_.end())
				j->second = value;
		}
	}

	const Variant& YumeRenderPipeline::GetShaderParameter(const YumeString& name) const
	{
		YumeHash nameHash(name);

		for(unsigned i = 0; i < commands_.size(); ++i)
		{
			YumeMap<YumeHash,Variant>::const_iterator j = commands_[i].shaderParameters_.find(nameHash);
			if(j != commands_[i].shaderParameters_.end())
				return j->second;
		}

		return Variant::EMPTY;
	}
}
