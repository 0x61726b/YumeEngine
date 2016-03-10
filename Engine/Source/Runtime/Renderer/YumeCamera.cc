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
#include "YumeCamera.h"
#include "YumeAuxRenderer.h"
#include "Scene/YumeSceneNode.h"
#include "YumeDrawable.h"

#undef near
#undef far


namespace YumeEngine
{

	extern const char* SCENE_CATEGORY;

	static const char* fillModeNames[] =
	{
		"Solid",
		"Wireframe",
		"Point",
		0
	};

	static const Matrix4 flipMatrix(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,-1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
		);

	YumeHash YumeCamera::cameraType_ = "Camera";

	YumeCamera::YumeCamera():
		viewDirty_(true),
		projectionDirty_(true),
		frustumDirty_(true),
		orthographic_(false),
		nearClip_(DEFAULT_NEARCLIP),
		farClip_(DEFAULT_FARCLIP),
		fov_(DEFAULT_CAMERA_FOV),
		orthoSize_(DEFAULT_ORTHOSIZE),
		aspectRatio_(1.0f),
		zoom_(1.0f),
		lodBias_(1.0f),
		viewMask_(DEFAULT_VIEWMASK),
		viewOverrideFlags_(VO_NONE),
		fillMode_(FILL_SOLID),
		projectionOffset_(Vector2::ZERO),
		reflectionPlane_(Plane::UP),
		clipPlane_(Plane::UP),
		autoAspectRatio_(true),
		flipVertical_(false),
		useReflection_(false),
		useClipping_(false)
	{
		reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
		typeName_ = "Camera";
		type_ = "Camera";
	}

	YumeCamera::~YumeCamera()
	{
	}

	void YumeCamera::DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest)
	{
		debug->AddFrustum(GetFrustum(),YumeColor::WHITE,depthTest);
	}

	void YumeCamera::SetNearClip(float nearClip)
	{
		nearClip_ = Max(nearClip,M_MIN_NEARCLIP);
		frustumDirty_ = true;
		projectionDirty_ = true;
	}

	void YumeCamera::SetFarClip(float farClip)
	{
		farClip_ = Max(farClip,M_MIN_NEARCLIP);
		frustumDirty_ = true;
		projectionDirty_ = true;
	}

	void YumeCamera::SetFov(float fov)
	{
		fov_ = Clamp(fov,0.0f,M_MAX_FOV);
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetOrthoSize(float orthoSize)
	{
		orthoSize_ = orthoSize;
		aspectRatio_ = 1.0f;
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetOrthoSize(const Vector2& orthoSize)
	{
		autoAspectRatio_ = false;
		orthoSize_ = orthoSize.y_;
		aspectRatio_ = orthoSize.x_ / orthoSize.y_;
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetAspectRatio(float aspectRatio)
	{
		autoAspectRatio_ = false;
		SetAspectRatioInternal(aspectRatio);
	}

	void YumeCamera::SetZoom(float zoom)
	{
		zoom_ = Max(zoom,M_EPSILON);
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetLodBias(float bias)
	{
		lodBias_ = Max(bias,M_EPSILON);
		
	}

	void YumeCamera::SetViewMask(unsigned mask)
	{
		viewMask_ = mask;
		
	}

	void YumeCamera::SetViewOverrideFlags(unsigned flags)
	{
		viewOverrideFlags_ = flags;
		
	}

	void YumeCamera::SetFillMode(FillMode mode)
	{
		fillMode_ = mode;
		
	}

	void YumeCamera::SetOrthographic(bool enable)
	{
		orthographic_ = enable;
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetAutoAspectRatio(bool enable)
	{
		autoAspectRatio_ = enable;
		
	}

	void YumeCamera::SetProjectionOffset(const Vector2& offset)
	{
		projectionOffset_ = offset;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetUseReflection(bool enable)
	{
		useReflection_ = enable;
		viewDirty_ = true;
		frustumDirty_ = true;
		
	}

	void YumeCamera::SetReflectionPlane(const Plane& plane)
	{
		reflectionPlane_ = plane;
		reflectionMatrix_ = reflectionPlane_.ReflectionMatrix();
		viewDirty_ = true;
		frustumDirty_ = true;
		
	}

	void YumeCamera::SetUseClipping(bool enable)
	{
		useClipping_ = enable;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetClipPlane(const Plane& plane)
	{
		clipPlane_ = plane;
		projectionDirty_ = true;
		
	}


	void YumeCamera::SetFlipVertical(bool enable)
	{
		flipVertical_ = enable;
		projectionDirty_ = true;
		
	}

	float YumeCamera::GetNearClip() const
	{
		// Orthographic camera has always near clip at 0 to avoid trouble with shader depth parameters,
		// and unlike in perspective mode there should be no depth buffer precision issue
		if(!orthographic_)
			return nearClip_;
		else
			return 0.0f;
	}

	Frustum YumeCamera::GetSplitFrustum(float nearClip,float farClip) const
	{
		Frustum ret;

		Matrix3x4 worldTransform = GetEffectiveWorldTransform();
		nearClip = Max(nearClip,GetNearClip());
		farClip = Min(farClip,farClip_);
		if(farClip < nearClip)
			farClip = nearClip;

		if(!orthographic_)
			ret.Define(fov_,aspectRatio_,zoom_,nearClip,farClip,worldTransform);
		else
			ret.DefineOrtho(orthoSize_,aspectRatio_,zoom_,nearClip,farClip,worldTransform);

		return ret;
	}

	Frustum YumeCamera::GetViewSpaceFrustum() const
	{
		Frustum ret;

		if(!orthographic_)
			ret.Define(fov_,aspectRatio_,zoom_,GetNearClip(),farClip_);
		else
			ret.DefineOrtho(orthoSize_,aspectRatio_,zoom_,GetNearClip(),farClip_);

		return ret;
	}

	Frustum YumeCamera::GetViewSpaceSplitFrustum(float nearClip,float farClip) const
	{
		Frustum ret;

		nearClip = Max(nearClip,GetNearClip());
		farClip = Min(farClip,farClip_);
		if(farClip < nearClip)
			farClip = nearClip;

		if(!orthographic_)
			ret.Define(fov_,aspectRatio_,zoom_,nearClip,farClip);
		else
			ret.DefineOrtho(orthoSize_,aspectRatio_,zoom_,nearClip,farClip);

		return ret;
	}

	Ray YumeCamera::GetScreenRay(float x,float y) const
	{
		Ray ret;

		// If projection is invalid, just return a ray pointing forward
		if(!IsProjectionValid())
		{
			ret.origin_ = node_ ? node_->GetWorldPosition() : Vector3::ZERO;
			ret.direction_ = node_ ? node_->GetWorldDirection() : Vector3::FORWARD;
			return ret;
		}

		Matrix4 viewProjInverse = (GetProjection(false) * GetView()).Inverse();

		// The parameters range from 0.0 to 1.0. Expand to normalized device coordinates (-1.0 to 1.0) & flip Y axis
		x = 2.0f * x - 1.0f;
		y = 1.0f - 2.0f * y;
		Vector3 near(x,y,0.0f);
		Vector3 far(x,y,1.0f);

		ret.origin_ = viewProjInverse * near;
		ret.direction_ = ((viewProjInverse * far) - ret.origin_).Normalized();
		return ret;
	}

	Vector2 YumeCamera::WorldToScreenPoint(const Vector3& worldPos) const
	{
		Vector3 eyeSpacePos = GetView() * worldPos;
		Vector2 ret;

		if(eyeSpacePos.z_ > 0.0f)
		{
			Vector3 screenSpacePos = GetProjection(false) * eyeSpacePos;
			ret.x_ = screenSpacePos.x_;
			ret.y_ = screenSpacePos.y_;
		}
		else
		{
			ret.x_ = (-eyeSpacePos.x_ > 0.0f) ? -1.0f : 1.0f;
			ret.y_ = (-eyeSpacePos.y_ > 0.0f) ? -1.0f : 1.0f;
		}

		ret.x_ = (ret.x_ / 2.0f) + 0.5f;
		ret.y_ = 1.0f - ((ret.y_ / 2.0f) + 0.5f);
		return ret;
	}

	Vector3 YumeCamera::ScreenToWorldPoint(const Vector3& screenPos) const
	{
		Ray ray = GetScreenRay(screenPos.x_,screenPos.y_);
		Vector3 viewSpaceDir = (GetView() * Vector4(ray.direction_,0.0f));
		float rayDistance = (Max(screenPos.z_ - GetNearClip(),0.0f) / viewSpaceDir.z_);
		return ray.origin_ + ray.direction_ * rayDistance;
	}

	const Frustum& YumeCamera::GetFrustum() const
	{
		if(frustumDirty_)
		{
			Matrix3x4 worldTransform = GetEffectiveWorldTransform();

			if(!orthographic_)
				frustum_.Define(fov_,aspectRatio_,zoom_,GetNearClip(),farClip_,worldTransform);
			else
				frustum_.DefineOrtho(orthoSize_,aspectRatio_,zoom_,GetNearClip(),farClip_,worldTransform);

			frustumDirty_ = false;
		}

		return frustum_;
	}

	const Matrix4& YumeCamera::GetProjection() const
	{
		if(projectionDirty_)
		{
			projection_ = GetProjection(true);
			projectionDirty_ = false;
		}

		return projection_;
	}

	Matrix4 YumeCamera::GetProjection(bool apiSpecific) const
	{
		Matrix4 ret(Matrix4::ZERO);

		// Whether to construct matrix using OpenGL or Direct3D clip space convention
#ifdef URHO3D_OPENGL
		bool openGLFormat = apiSpecific;
#else
		bool openGLFormat = false;
#endif

		if(!orthographic_)
		{
			float nearClip = GetNearClip();
			float h = (1.0f / tanf(fov_ * M_DEGTORAD * 0.5f)) * zoom_;
			float w = h / aspectRatio_;
			float q,r;

			if(openGLFormat)
			{
				q = (farClip_ + nearClip) / (farClip_ - nearClip);
				r = -2.0f * farClip_ * nearClip / (farClip_ - nearClip);
			}
			else
			{
				q = farClip_ / (farClip_ - nearClip);
				r = -q * nearClip;
			}

			ret.m00_ = w;
			ret.m02_ = projectionOffset_.x_ * 2.0f;
			ret.m11_ = h;
			ret.m12_ = projectionOffset_.y_ * 2.0f;
			ret.m22_ = q;
			ret.m23_ = r;
			ret.m32_ = 1.0f;
		}
		else
		{
			// Disregard near clip, because it does not affect depth precision as with perspective projection
			float h = (1.0f / (orthoSize_ * 0.5f)) * zoom_;
			float w = h / aspectRatio_;
			float q,r;

			if(openGLFormat)
			{
				q = 2.0f / farClip_;
				r = -1.0f;
			}
			else
			{
				q = 1.0f / farClip_;
				r = 0.0f;
			}

			ret.m00_ = w;
			ret.m03_ = projectionOffset_.x_ * 2.0f;
			ret.m11_ = h;
			ret.m13_ = projectionOffset_.y_ * 2.0f;
			ret.m22_ = q;
			ret.m23_ = r;
			ret.m33_ = 1.0f;
		}

		if(flipVertical_)
			ret = flipMatrix * ret;

		return ret;
	}

	void YumeCamera::GetFrustumSize(Vector3& near,Vector3& far) const
	{
		near.z_ = GetNearClip();
		far.z_ = farClip_;

		if(!orthographic_)
		{
			float halfViewSize = tanf(fov_ * M_DEGTORAD * 0.5f) / zoom_;
			near.y_ = near.z_ * halfViewSize;
			near.x_ = near.y_ * aspectRatio_;
			far.y_ = far.z_ * halfViewSize;
			far.x_ = far.y_ * aspectRatio_;
		}
		else
		{
			float halfViewSize = orthoSize_ * 0.5f / zoom_;
			near.y_ = far.y_ = halfViewSize;
			near.x_ = far.x_ = near.y_ * aspectRatio_;
		}

		if(flipVertical_)
		{
			near.y_ = -near.y_;
			far.y_ = -far.y_;
		}
	}

	float YumeCamera::GetHalfViewSize() const
	{
		if(!orthographic_)
			return tanf(fov_ * M_DEGTORAD * 0.5f) / zoom_;
		else
			return orthoSize_ * 0.5f / zoom_;
	}

	float YumeCamera::GetDistance(const Vector3& worldPos) const
	{
		if(!orthographic_)
		{
			const Vector3& cameraPos = node_ ? node_->GetWorldPosition() : Vector3::ZERO;
			return (worldPos - cameraPos).Length();
		}
		else
			return Abs((GetView() * worldPos).z_);
	}

	float YumeCamera::GetDistanceSquared(const Vector3& worldPos) const
	{
		if(!orthographic_)
		{
			const Vector3& cameraPos = node_ ? node_->GetWorldPosition() : Vector3::ZERO;
			return (worldPos - cameraPos).LengthSquared();
		}
		else
		{
			float distance = (GetView() * worldPos).z_;
			return distance * distance;
		}
	}

	float YumeCamera::GetLodDistance(float distance,float scale,float bias) const
	{
		float d = Max(lodBias_ * bias * scale * zoom_,M_EPSILON);
		if(!orthographic_)
			return distance / d;
		else
			return orthoSize_ / d;
	}

	Quaternion YumeCamera::GetFaceCameraRotation(const Vector3& position,const Quaternion& rotation,FaceCameraMode mode)
	{
		if(!node_)
			return rotation;

		switch(mode)
		{
		default:
			return rotation;

		case FC_ROTATE_XYZ:
			return node_->GetWorldRotation();

		case FC_ROTATE_Y:
		{
			Vector3 euler = rotation.EulerAngles();
			euler.y_ = node_->GetWorldRotation().EulerAngles().y_;
			return Quaternion(euler.x_,euler.y_,euler.z_);
		}

		case FC_LOOKAT_XYZ:
		{
			Quaternion lookAt;
			lookAt.FromLookRotation(position - node_->GetWorldPosition());
			return lookAt;
		}

		case FC_LOOKAT_Y:
		{
			// Make the Y-only lookat happen on an XZ plane to make sure there are no unwanted transitions
			// or singularities
			Vector3 lookAtVec(position - node_->GetWorldPosition());
			lookAtVec.y_ = 0.0f;

			Quaternion lookAt;
			lookAt.FromLookRotation(lookAtVec);

			Vector3 euler = rotation.EulerAngles();
			euler.y_ = lookAt.EulerAngles().y_;
			return Quaternion(euler.x_,euler.y_,euler.z_);
		}
		}
	}

	Matrix3x4 YumeCamera::GetEffectiveWorldTransform() const
	{
		Matrix3x4 worldTransform = node_ ? Matrix3x4(node_->GetWorldPosition(),node_->GetWorldRotation(),1.0f) : Matrix3x4::IDENTITY;
		return useReflection_ ? reflectionMatrix_ * worldTransform : worldTransform;
	}

	bool YumeCamera::IsProjectionValid() const
	{
		return farClip_ > GetNearClip();
	}

	const Matrix3x4& YumeCamera::GetView() const
	{
		if(viewDirty_)
		{
			// Note: view matrix is unaffected by node or parent scale
			view_ = GetEffectiveWorldTransform().Inverse();
			viewDirty_ = false;
		}

		return view_;
	}

	void YumeCamera::SetAspectRatioInternal(float aspectRatio)
	{
		aspectRatio_ = aspectRatio;
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetOrthoSizeAttr(float orthoSize)
	{
		orthoSize_ = orthoSize;
		frustumDirty_ = true;
		projectionDirty_ = true;
		
	}

	void YumeCamera::SetReflectionPlaneAttr(const Vector4& value)
	{
		SetReflectionPlane(Plane(value));
	}

	void YumeCamera::SetClipPlaneAttr(const Vector4& value)
	{
		SetClipPlane(Plane(value));
	}

	Vector4 YumeCamera::GetReflectionPlaneAttr() const
	{
		return reflectionPlane_.ToVector4();
	}

	Vector4 YumeCamera::GetClipPlaneAttr() const
	{
		return clipPlane_.ToVector4();
	}

	void YumeCamera::OnNodeSet(YumeSceneNode* node)
	{
		if(node)
			node->AddListener(this);
	}

	void YumeCamera::OnMarkedDirty(YumeSceneNode* node)
	{
		frustumDirty_ = true;
		viewDirty_ = true;
	}
}
