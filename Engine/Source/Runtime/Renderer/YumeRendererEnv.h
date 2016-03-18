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
#ifndef __YumeRendererEnv_h__
#define __YumeRendererEnv_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeDrawable.h"
#include "Renderer/YumeTexture.h"
#include "Math/YumeColor.h"
#include "Math/YumeMatrix3x4.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{


	class YumeAPIExport YumeRendererEnvironment : public YumeDrawable
	{
	public:
		YumeRendererEnvironment();
		virtual ~YumeRendererEnvironment();
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;


		void SetBoundingBox(const BoundingBox& box);

		void SetAmbientColor(const YumeColor& color);

		void SetFogColor(const YumeColor& color);

		void SetFogStart(float start);

		void SetFogEnd(float end);

		void SetFogHeight(float height);

		void SetFogHeightScale(float scale);

		void SetPriority(int priority);

		void SetHeightFog(bool enable);

		void SetOverride(bool enable);

		void SetAmbientGradient(bool enable);

		void SetZoneTexture(YumeTexture* texture);


		const Matrix3x4& GetInverseWorldTransform() const;


		const YumeColor& GetAmbientColor() const { return ambientColor_; }


		const YumeColor& GetAmbientStartColor();

		const YumeColor& GetAmbientEndColor();


		const YumeColor& GetFogColor() const { return fogColor_; }


		float GetFogStart() const { return fogStart_; }


		float GetFogEnd() const { return fogEnd_; }


		float GetFogHeight() const { return fogHeight_; }


		float GetFogHeightScale() const { return fogHeightScale_; }


		int GetPriority() const { return priority_; }


		bool GetHeightFog() const { return heightFog_; }


		bool GetOverride() const { return override_; }


		bool GetAmbientGradient() const { return ambientGradient_; }


		YumeTexture* GetZoneTexture() const { return zoneTexture_; }


		bool IsInside(const Vector3& point) const;


	protected:

		virtual void OnMarkedDirty(YumeSceneNode* node);

		virtual void OnWorldBoundingBoxUpdate();

		virtual void OnRemoveFromOctree();

		void UpdateAmbientGradient();

		void ClearDrawablesZone();


		mutable Matrix3x4 inverseWorld_;

		mutable bool inverseWorldDirty_;

		bool heightFog_;

		bool override_;

		bool ambientGradient_;

		BoundingBox lastWorldBoundingBox_;

		YumeColor ambientColor_;

		YumeColor ambientStartColor_;

		YumeColor ambientEndColor_;

		YumeColor fogColor_;

		float fogStart_;

		float fogEnd_;

		float fogHeight_;

		float fogHeightScale_;

		int priority_;

		SharedPtr<YumeTexture> zoneTexture_;

		SharedPtr<YumeRendererEnvironment> lastAmbientStartZone_;

		SharedPtr<YumeRendererEnvironment> lastAmbientEndZone_;
	};
}


//----------------------------------------------------------------------------
#endif
