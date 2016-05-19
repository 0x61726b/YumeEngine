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
#include "YumeLPVCamera.h"
#include "Input/YumeInput.h"

using namespace DirectX;

namespace YumeEngine
{
	YumeCamera::YumeCamera(float nearClip,float farClip): nearZ(nearClip),
		farZ(farClip)
	{
		_ASSERT(nearZ > 0 && nearZ < farZ);
		_ASSERT(farZ > 0 && farZ > nearZ);

		world = XMMatrixIdentity();
		view = XMMatrixIdentity();
		position = XMLoadFloat3(&XMFLOAT3(0,0,0));
		orientation = XMQuaternionIdentity();
	}

	YumeCamera::~YumeCamera()
	{

	}

	void YumeCamera::Update(float dt)
	{



	}

	void YumeCamera::WorldMatrixChanged()
	{
		XMVECTOR det;
		view = XMMatrixInverse(&det,world);
		viewProjection = XMMatrixMultiply(view,projection);
	}

	XMVECTOR YumeCamera::Forward() const
	{
		return ForwardVec(world);
	}

	XMVECTOR YumeCamera::Back() const
	{
		return BackVec(world);
	}

	XMVECTOR YumeCamera::Up() const
	{
		return UpVec(world);
	}

	XMVECTOR YumeCamera::Down() const
	{
		return DownVec(world);
	}

	XMVECTOR YumeCamera::Right() const
	{
		return RightVec(world);
	}

	XMVECTOR YumeCamera::Left() const
	{
		return LeftVec(world);
	}

	void YumeCamera::SetLookAt(const XMFLOAT3 &eye,const XMFLOAT3 &lookAt,const XMFLOAT3 &up)
	{
		XMVECTOR det;
		view = XMMatrixLookAtLH(XMLoadFloat3(&eye),XMLoadFloat3(&lookAt),XMLoadFloat3(&up));
		world = XMMatrixInverse(&det,view);
		position = XMLoadFloat3(&eye);
		orientation = XMQuaternionRotationMatrix(world);

		WorldMatrixChanged();
	}

	void YumeCamera::SetWorldMatrix(const XMMATRIX& newWorld)
	{
		world = newWorld;
		position = TranslationVec(world);
		orientation = XMQuaternionRotationMatrix(world);

		WorldMatrixChanged();
	}

	void YumeCamera::SetPosition(const XMVECTOR& newPosition)
	{
		position = newPosition;
		SetTranslationVec(world,newPosition);

		WorldMatrixChanged();
	}

	void YumeCamera::SetOrientation(const XMVECTOR& newOrientation)
	{
		world = XMMatrixRotationQuaternion(newOrientation);
		orientation = newOrientation;
		SetTranslationVec(world,position);

		WorldMatrixChanged();
	}

	void YumeCamera::SetNearClip(float newNearClip)
	{
		nearZ = newNearClip;
		CreateProjection();
	}

	void YumeCamera::SetFarClip(float newFarClip)
	{
		farZ = newFarClip;
		CreateProjection();
	}

	void YumeCamera::SetProjection(const XMMATRIX& newProjection)
	{
		projection = newProjection;
		viewProjection = XMMatrixMultiply(view,projection);
	}

	//=========================================================================================
	// OrthographicCamera
	//=========================================================================================

	OrthographicCamera::OrthographicCamera(float minX,float minY,float maxX,
		float maxY,float nearClip,float farClip): YumeCamera(nearClip,farClip),
		xMin(minX),
		yMin(minY),
		xMax(maxX),
		yMax(maxY)

	{
		_ASSERT(xMax > xMin && yMax > yMin);

		CreateProjection();
	}

	OrthographicCamera::~OrthographicCamera()
	{

	}

	void OrthographicCamera::CreateProjection()
	{
		projection = XMMatrixOrthographicOffCenterLH(xMin,xMax,yMin,yMax,nearZ,farZ);
		viewProjection = XMMatrixMultiply(view,projection);
	}

	void OrthographicCamera::SetMinX(float minX)
	{
		xMin = minX;
		CreateProjection();
	}

	void OrthographicCamera::SetMinY(float minY)
	{
		yMin = minY;
		CreateProjection();
	}

	void OrthographicCamera::SetMaxX(float maxX)
	{
		xMax = maxX;
		CreateProjection();
	}

	void OrthographicCamera::SetMaxY(float maxY)
	{
		yMax = maxY;
		CreateProjection();
	}

	//=========================================================================================
	// PerspectiveCamera
	//=========================================================================================

	PerspectiveCamera::PerspectiveCamera(float aspectRatio,float fieldOfView,
		float nearClip,float farClip): YumeCamera(nearClip,farClip),
		aspect(aspectRatio),
		fov(fieldOfView)
	{
		_ASSERT(aspectRatio > 0);
		_ASSERT(fieldOfView > 0 && fieldOfView <= 3.14159f);
		CreateProjection();
	}

	PerspectiveCamera::~PerspectiveCamera()
	{

	}

	void PerspectiveCamera::SetAspectRatio(float aspectRatio)
	{
		aspect = aspectRatio;
		CreateProjection();
	}

	void PerspectiveCamera::SetFieldOfView(float fieldOfView)
	{
		fov = fieldOfView;
		CreateProjection();
	}

	void PerspectiveCamera::CreateProjection()
	{
		projection = XMMatrixPerspectiveFovLH(fov,aspect,nearZ,farZ);
		viewProjection = XMMatrixMultiply(view,projection);
	}

	//=========================================================================================
	// FirstPersonCamera
	//=========================================================================================

	FirstPersonCamera::FirstPersonCamera(float aspectRatio,float fieldOfView,
		float nearClip,float farClip): PerspectiveCamera(aspectRatio,fieldOfView,
		nearClip,farClip),
		xRot(0),
		yRot(0)
	{

	}

	FirstPersonCamera::~FirstPersonCamera()
	{

	}

	void FirstPersonCamera::SetXRotation(float xRotation)
	{
		xRot = Clamp(xRotation,-XM_PIDIV2,XM_PIDIV2);
		SetOrientation(XMQuaternionRotationRollPitchYaw(xRot,yRot,0));
	}

	void FirstPersonCamera::SetYRotation(float yRotation)
	{
		yRot = XMScalarModAngle(yRotation);
		SetOrientation(XMQuaternionRotationRollPitchYaw(xRot,yRot,0));
	}
}
