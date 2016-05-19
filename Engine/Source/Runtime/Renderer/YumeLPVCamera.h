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
#ifndef __YumeLPVCamera_h__
#define __YumeLPVCamera_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include <DirectXMath.h>
//----------------------------------------------------------------------------
namespace YumeEngine
{

	// Returns the forward vector from a transform matrix
	inline DirectX::XMVECTOR ForwardVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m._31,m._32,m._33));
	}

	// Returns the forward vector from a transform matrix
	inline DirectX::XMVECTOR BackVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(-m._31,-m._32,-m._33));
	}

	// Returns the forward vector from a transform matrix
	inline DirectX::XMVECTOR RightVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m._11,m._12,m._13));
	}

	// Returns the forward vector from a transform matrix
	inline DirectX::XMVECTOR LeftVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(-m._11,-m._12,-m._13));
	}

	// Returns the forward vector from a transform matrix
	inline DirectX::XMVECTOR UpVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m._21,m._22,m._23));
	}

	// Returns the forward vector from a transform matrix
	inline DirectX::XMVECTOR DownVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(-m._21,-m._22,-m._23));
	}

	// Returns the translation vector from a transform matrix
	inline DirectX::XMVECTOR TranslationVec(const DirectX::XMMATRIX& matrix)
	{
		DirectX::XMFLOAT4X4 m;
		XMStoreFloat4x4(&m,matrix);
		return DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(m._41,m._42,m._43));
	}

	// Sets the translation vector in a transform matrix
	inline void SetTranslationVec(DirectX::XMMATRIX& matrix,const DirectX::XMVECTOR& translation)
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m,matrix);
		m._41 = DirectX::XMVectorGetX(translation);
		m._42 = DirectX::XMVectorGetY(translation);
		m._43 = DirectX::XMVectorGetZ(translation);

		matrix = DirectX::XMLoadFloat4x4(&m);
	}
	class YumeAPIExport YumeCamera
		: public YumeBase
	{

	protected:

		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX viewProjection;

		DirectX::XMMATRIX world;
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR orientation;

		float nearZ;
		float farZ;

		virtual void CreateProjection() = 0;
		void WorldMatrixChanged();

	public:

		YumeCamera(float nearZ,float farZ);
		~YumeCamera();

		void Update(float dt);

		const DirectX::XMMATRIX& ViewMatrix() const { return view; };
		const DirectX::XMMATRIX& ProjectionMatrix() const { return projection; };
		const DirectX::XMMATRIX& ViewProjectionMatrix() const { return viewProjection; };
		const DirectX::XMMATRIX& WorldMatrix() const { return world; };
		const DirectX::XMVECTOR& Position() const { return position; };
		const DirectX::XMVECTOR& Orientation() const { return orientation; };
		const float& NearClip() const { return nearZ; };
		const float& FarClip() const { return farZ; };

		DirectX::XMVECTOR Forward() const;
		DirectX::XMVECTOR Back() const;
		DirectX::XMVECTOR Up() const;
		DirectX::XMVECTOR Down() const;
		DirectX::XMVECTOR Right() const;
		DirectX::XMVECTOR Left() const;

		void SetLookAt(const DirectX::XMFLOAT3& eye,const DirectX::XMFLOAT3& lookAt,const DirectX::XMFLOAT3& up);
		void SetWorldMatrix(const DirectX::XMMATRIX& newWorld);
		void SetPosition(const DirectX::XMVECTOR& newPosition);
		void SetOrientation(const DirectX::XMVECTOR& newOrientation);
		void SetNearClip(float newNearClip);
		void SetFarClip(float newFarClip);
		void SetProjection(const DirectX::XMMATRIX& newProjection);
	};


	// Camera with an orthographic projection
	class OrthographicCamera : public YumeCamera
	{

	protected:

		float xMin;
		float xMax;
		float yMin;
		float yMax;

		virtual void CreateProjection();

	public:

		OrthographicCamera(float minX,float minY,float maxX,float maxY,float nearClip,float farClip);
		~OrthographicCamera();

		float MinX() const { return xMin; };
		float MinY() const { return yMin; };
		float MaxX() const { return xMax; };
		float MaxY() const { return yMax; };

		void SetMinX(float minX);
		void SetMinY(float minY);
		void SetMaxX(float maxX);
		void SetMaxY(float maxY);
	};

	// Camera with a perspective projection
	class PerspectiveCamera : public YumeCamera
	{

	protected:

		float aspect;
		float fov;

		virtual void CreateProjection();

	public:

		PerspectiveCamera(float aspectRatio,float fieldOfView,float nearClip,float farClip);
		~PerspectiveCamera();

		float AspectRatio() const { return aspect; };
		float FieldOfView() const { return fov; };

		void SetAspectRatio(float aspectRatio);
		void SetFieldOfView(float fieldOfView);
	};

	// Perspective camera that rotates about Z and Y axes
	class FirstPersonCamera : public PerspectiveCamera
	{

	protected:

		float xRot;
		float yRot;

	public:

		FirstPersonCamera(float aspectRatio,float fieldOfView,float nearClip,float farClip);
		~FirstPersonCamera();

		float XRotation() const { return xRot; };
		float YRotation() const { return yRot; };

		void SetXRotation(float xRotation);
		void SetYRotation(float yRotation);
	};

}


//----------------------------------------------------------------------------
#endif
