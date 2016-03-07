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
#ifndef __YumeCamera_h__
#define __YumeCamera_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Scene/YumeSceneComponent.h"
#include "Math/YumeFrustum.h"
#include "Math/YumeRay.h"
#include "Renderer/YumeRendererDefs.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	static const float DEFAULT_NEARCLIP = 0.1f;
	static const float DEFAULT_FARCLIP = 1000.0f;
	static const float DEFAULT_CAMERA_FOV = 45.0f;
	static const float DEFAULT_ORTHOSIZE = 20.0f;

	static const unsigned VO_NONE = 0x0;
	static const unsigned VO_LOW_MATERIAL_QUALITY = 0x1;
	static const unsigned VO_DISABLE_SHADOWS = 0x2;
	static const unsigned VO_DISABLE_OCCLUSION = 0x4;

	/// %Camera component.
	class YumeAPIExport YumeCamera : public YumeSceneComponent
	{
	public:
		YumeCamera();
		virtual ~YumeCamera();


		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		void SetNearClip(float nearClip);
		void SetFarClip(float farClip);
		void SetFov(float fov);
		void SetOrthoSize(float orthoSize);
		void SetOrthoSize(const Vector2& orthoSize);
		void SetAspectRatio(float aspectRatio);
		void SetFillMode(FillMode mode);
		void SetZoom(float zoom);
		void SetLodBias(float bias);
		void SetViewMask(unsigned mask);
		void SetViewOverrideFlags(unsigned flags);
		void SetOrthographic(bool enable);
		void SetAutoAspectRatio(bool enable);
		void SetProjectionOffset(const Vector2& offset);
		void SetUseReflection(bool enable);
		void SetReflectionPlane(const Plane& plane);
		void SetUseClipping(bool enable);
		void SetClipPlane(const Plane& plane);
		void SetFlipVertical(bool enable);

		float GetFarClip() const { return farClip_; }

		float GetNearClip() const;

		float GetFov() const { return fov_; }

		float GetOrthoSize() const { return orthoSize_; }

		float GetAspectRatio() const { return aspectRatio_; }

		float GetZoom() const { return zoom_; }

		float GetLodBias() const { return lodBias_; }

		unsigned GetViewMask() const { return viewMask_; }

		unsigned GetViewOverrideFlags() const { return viewOverrideFlags_; }

		FillMode GetFillMode() const { return fillMode_; }

		bool IsOrthographic() const { return orthographic_; }

		bool GetAutoAspectRatio() const { return autoAspectRatio_; }

		const Frustum& GetFrustum() const;
		const Matrix4& GetProjection() const;
		Matrix4 GetProjection(bool apiSpecific) const;
		const Matrix3x4& GetView() const;
		void GetFrustumSize(Vector3& near,Vector3& far) const;
		float GetHalfViewSize() const;
		Frustum GetSplitFrustum(float nearClip,float farClip) const;
		Frustum GetViewSpaceFrustum() const;
		Frustum GetViewSpaceSplitFrustum(float nearClip,float farClip) const;
		Ray GetScreenRay(float x,float y) const;
		Vector2 WorldToScreenPoint(const Vector3& worldPos) const;
		Vector3 ScreenToWorldPoint(const Vector3& screenPos) const;

		const Vector2& GetProjectionOffset() const { return projectionOffset_; }

		bool GetUseReflection() const { return useReflection_; }

		const Plane& GetReflectionPlane() const { return reflectionPlane_; }
		bool GetUseClipping() const { return useClipping_; }

		const Plane& GetClipPlane() const { return clipPlane_; }

		bool GetFlipVertical() const { return flipVertical_; }

		bool GetReverseCulling() const { return flipVertical_ ^ useReflection_; }
		float GetDistance(const Vector3& worldPos) const;
		float GetDistanceSquared(const Vector3& worldPos) const;
		float GetLodDistance(float distance,float scale,float bias) const;

		Quaternion GetFaceCameraRotation(const Vector3& position,const Quaternion& rotation,FaceCameraMode mode);
		Matrix3x4 GetEffectiveWorldTransform() const;

		bool IsProjectionValid() const;
		void SetAspectRatioInternal(float aspectRatio);
		void SetOrthoSizeAttr(float orthoSize);
		void SetReflectionPlaneAttr(const Vector4& value);

		Vector4 GetReflectionPlaneAttr() const;
		void SetClipPlaneAttr(const Vector4& value);
		Vector4 GetClipPlaneAttr() const;

	protected:
		virtual void OnNodeSet(YumeSceneNode* node);
		virtual void OnMarkedDirty(YumeSceneNode* node);

	private:
		mutable Matrix3x4 view_;
		mutable Matrix4 projection_;
		mutable Frustum frustum_;
		mutable bool viewDirty_;
		mutable bool projectionDirty_;
		mutable bool frustumDirty_;
		bool orthographic_;
		float nearClip_;
		float farClip_;
		float fov_;
		float orthoSize_;
		float aspectRatio_;
		float zoom_;
		float lodBias_;
		unsigned viewMask_;
		unsigned viewOverrideFlags_;
		FillMode fillMode_;
		Vector2 projectionOffset_;
		Plane reflectionPlane_;
		Plane clipPlane_;
		Matrix3x4 reflectionMatrix_;
		bool autoAspectRatio_;
		bool flipVertical_;
		bool useReflection_;
		bool useClipping_;
	};
}


//----------------------------------------------------------------------------
#endif
