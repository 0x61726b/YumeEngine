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
#ifndef __YumeRenderPipeline_h__
#define __YumeRenderPipeline_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeColor.h"
#include "Renderer/YumeRendererDefs.h"
#include "Core/YumeXmlParser.h"

#include "Core/YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeXmlFile;

	
	enum RenderCommandType
	{
		CMD_NONE = 0,
		CMD_CLEAR,
		CMD_SCENEPASS,
		CMD_QUAD,
		CMD_FORWARDLIGHTS,
		CMD_LIGHTVOLUMES,
		CMD_RENDERUI
	};

	
	enum RenderCommandSortMode
	{
		SORT_FRONTTOBACK = 0,
		SORT_BACKTOFRONT
	};

	
	enum RenderTargetSizeMode
	{
		SIZE_ABSOLUTE = 0,
		SIZE_VIEWPORTDIVISOR,
		SIZE_VIEWPORTMULTIPLIER
	};

	
	struct YumeAPIExport RenderTargetInfo
	{
		
		RenderTargetInfo():
			size_(Vector2::ZERO),
			sizeMode_(SIZE_ABSOLUTE),
			enabled_(true),
			cubemap_(false),
			filtered_(false),
			sRGB_(false),
			persistent_(false)
		{
		}

		
		void Load(const XmlNode& element);

		
		YumeString name_;
		
		YumeString tag_;
		
		unsigned format_;
		
		Vector2 size_;
		
		RenderTargetSizeMode sizeMode_;
		
		bool enabled_;
		
		bool cubemap_;
		
		bool filtered_;
		
		bool sRGB_;
		
		bool persistent_;
	};

	
	struct YumeAPIExport RenderCommand
	{
		
		RenderCommand():
			clearFlags_(0),
			blendMode_(BLEND_REPLACE),
			enabled_(true),
			useFogColor_(false),
			markToStencil_(false),
			useLitBase_(true),
			vertexLights_(false)
		{
		}

		
		void Load(const XmlNode& element);
		
		void SetTextureName(TextureUnit unit,const YumeString& name);
		
		void SetShaderParameter(const YumeString& name,const Variant& value);
		
		void RemoveShaderParameter(const YumeString& name);
		
		void SetNumOutputs(unsigned num);
		
		void SetOutput(unsigned index,const YumeString& name,CubeMapFace face = FACE_POSITIVE_X);
		
		void SetOutputName(unsigned index,const YumeString& name);
		
		void SetOutputFace(unsigned index,CubeMapFace face);
		
		void SetDepthStencilName(const YumeString& name);

		
		const YumeString& GetTextureName(TextureUnit unit) const;
		
		const Variant& GetShaderParameter(const YumeString& name) const;

		
		unsigned GetNumOutputs() const { return outputs_.size(); }

		
		const YumeString& GetOutputName(unsigned index) const;
		
		CubeMapFace GetOutputFace(unsigned index) const;

		
		const YumeString& GetDepthStencilName() const { return depthStencilName_; }

		
		YumeString tag_;
		
		RenderCommandType type_;
		
		RenderCommandSortMode sortMode_;
		
		YumeString pass_;
		
		unsigned passIndex_;
		
		YumeString metadata_;
		
		YumeString vertexShaderName_;
		
		YumeString pixelShaderName_;
		
		YumeString vertexShaderDefines_;
		
		YumeString pixelShaderDefines_;
		
		YumeString textureNames_[MAX_TEXTURE_UNITS];
		
		YumeMap<YumeHash,Variant>::type shaderParameters_;
		
		YumeVector<std::pair<YumeString,CubeMapFace> >::type outputs_;
		
		YumeString depthStencilName_;
		
		unsigned clearFlags_;
		
		YumeColor clearColor_;
		
		float clearDepth_;
		
		unsigned clearStencil_;
		
		BlendMode blendMode_;
		
		bool enabled_;
		
		bool useFogColor_;
		
		bool markToStencil_;
		
		bool useLitBase_;
		
		bool vertexLights_;
	};

	
	class YumeAPIExport YumeRenderPipeline : public YumeBase
	{
	public:
		
		YumeRenderPipeline();
		
		~YumeRenderPipeline();

		
		SharedPtr<YumeRenderPipeline> Clone();
		
		bool Load(YumeXmlFile* file);
		
		bool Append(YumeXmlFile* file);
		
		void SetEnabled(const YumeString& tag,bool active);
		
		void ToggleEnabled(const YumeString& tag);
		
		void SetRenderTarget(unsigned index,const RenderTargetInfo& info);
		
		void AddRenderTarget(const RenderTargetInfo& info);
		
		void RemoveRenderTarget(unsigned index);
		
		void RemoveRenderTarget(const YumeString& name);
		
		void RemoveRenderTargets(const YumeString& tag);
		
		void SetCommand(unsigned index,const RenderCommand& command);
		
		void AddCommand(const RenderCommand& command);
		
		void InsertCommand(unsigned index,const RenderCommand& command);
		
		void RemoveCommand(unsigned index);
		
		void RemoveCommands(const YumeString& tag);
		
		void SetShaderParameter(const YumeString& name,const Variant& value);

		
		unsigned GetNumRenderTargets() const { return renderTargets_.size(); }

		
		unsigned GetNumCommands() const { return commands_.size(); }

		
		RenderCommand* GetCommand(unsigned index) { return index < commands_.size() ? &commands_[index] : (RenderCommand*)0; }

		
		const Variant& GetShaderParameter(const YumeString& name) const;

		
		YumeVector<RenderTargetInfo>::type renderTargets_;
		
		YumeVector<RenderCommand>::type commands_;
	};
}


//----------------------------------------------------------------------------
#endif
