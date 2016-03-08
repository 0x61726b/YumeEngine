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

	/// %Component that describes global rendering properties.
	class YumeAPIExport YumeRendererEnvironment : public YumeDrawable
	{
	public:
		YumeRendererEnvironment();
		virtual ~YumeRendererEnvironment();
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		/// Set local-space bounding box. Will be used as an oriented bounding box to test whether objects or the camera are inside.
		void SetBoundingBox(const BoundingBox& box);
		/// Set ambient color
		void SetAmbientColor(const YumeColor& color);
		/// Set fog color.
		void SetFogColor(const YumeColor& color);
		/// Set fog start distance.
		void SetFogStart(float start);
		/// Set fog end distance.
		void SetFogEnd(float end);
		/// Set fog height distance relative to the scene node's world position. Effective only in height fog mode.
		void SetFogHeight(float height);
		/// Set fog height scale. Effective only in height fog mode.
		void SetFogHeightScale(float scale);
		/// Set zone priority. If an object or camera is inside several zones, the one with highest priority is used.
		void SetPriority(int priority);
		/// Set height fog mode.
		void SetHeightFog(bool enable);
		/// Set override mode. If camera is inside an override zone, that zone will be used for all rendered objects instead of their own zone.
		void SetOverride(bool enable);
		/// Set ambient gradient mode. In gradient mode ambient color is interpolated from neighbor zones.
		void SetAmbientGradient(bool enable);
		/// Set zone texture. This will be bound to the zone texture unit when rendering objects inside the zone. Note that the default shaders do not use it.
		void SetZoneTexture(YumeTexture* texture);

		/// Return inverse world transform.
		const Matrix3x4& GetInverseWorldTransform() const;

		/// Return zone's own ambient color, disregarding gradient mode.
		const YumeColor& GetAmbientColor() const { return ambientColor_; }

		/// Return ambient start color. Not safe to call from worker threads due to possible octree query.
		const YumeColor& GetAmbientStartColor();
		/// Return ambient end color. Not safe to call from worker threads due to possible octree query.
		const YumeColor& GetAmbientEndColor();

		/// Return fog color.
		const YumeColor& GetFogColor() const { return fogColor_; }

		/// Return fog start distance.
		float GetFogStart() const { return fogStart_; }

		/// Return fog end distance.
		float GetFogEnd() const { return fogEnd_; }

		/// Return fog height distance relative to the scene node's world position.
		float GetFogHeight() const { return fogHeight_; }

		/// Return fog height scale.
		float GetFogHeightScale() const { return fogHeightScale_; }

		/// Return zone priority.
		int GetPriority() const { return priority_; }

		/// Return whether height fog mode is enabled.
		bool GetHeightFog() const { return heightFog_; }

		/// Return whether override mode is enabled.
		bool GetOverride() const { return override_; }

		/// Return whether ambient gradient mode is enabled.
		bool GetAmbientGradient() const { return ambientGradient_; }

		/// Return zone texture.
		YumeTexture* GetZoneTexture() const { return zoneTexture_.get(); }

		/// Check whether a point is inside.
		bool IsInside(const Vector3& point) const;
	

	protected:
		/// Handle node transform being dirtied.
		virtual void OnMarkedDirty(YumeSceneNode* node);
		/// Recalculate the world-space bounding box.
		virtual void OnWorldBoundingBoxUpdate();
		/// Handle removal from octree.
		virtual void OnRemoveFromOctree();
		/// Recalculate the ambient gradient colors from neighbor zones. Not safe to call from worker threads due to octree query.
		void UpdateAmbientGradient();
		/// Clear zone reference from drawables inside the bounding box.
		void ClearDrawablesZone();

		/// Cached inverse world transform matrix.
		mutable Matrix3x4 inverseWorld_;
		/// Inverse transform dirty flag.
		mutable bool inverseWorldDirty_;
		/// Height fog mode flag.
		bool heightFog_;
		/// Override mode flag.
		bool override_;
		/// Ambient gradient mode flag.
		bool ambientGradient_;
		/// Last world-space bounding box.
		BoundingBox lastWorldBoundingBox_;
		/// Ambient color.
		YumeColor ambientColor_;
		/// Cached ambient start color.
		YumeColor ambientStartColor_;
		/// Cached ambient end color.
		YumeColor ambientEndColor_;
		/// Fog color.
		YumeColor fogColor_;
		/// Fog start distance.
		float fogStart_;
		/// Fog end distance.
		float fogEnd_;
		/// Fog height distance.
		float fogHeight_;
		/// Fog height cale.
		float fogHeightScale_;
		/// Zone priority.
		int priority_;
		/// Zone texture.
		SharedPtr<YumeTexture> zoneTexture_;
		/// Last zone used for ambient gradient start color.
		SharedPtr<YumeRendererEnvironment> lastAmbientStartZone_;
		/// Last zone used for ambient gradient end color.
		SharedPtr<YumeRendererEnvironment> lastAmbientEndZone_;
	};
}


//----------------------------------------------------------------------------
#endif
