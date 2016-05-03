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


namespace YumeEngine
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Default constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	void YumeLPVCamera::Update()
	{
		DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle,m_fCameraYawAngle,0);
		DirectX::XMStoreFloat4x4(&Rot,rot);
	}

	void YumeLPVCamera::GetFrustumSize(DirectX::XMFLOAT3& n,DirectX::XMFLOAT3& f)
	{
		n.z = GetNearClip();
		f.z = GetFarClip();

		
		float halfViewSize = tanf(m_fFOV * M_DEGTORAD * 0.5f);
		n.y = n.z * halfViewSize;
		n.x = n.y * m_fAspect;
		f.y = f.z * halfViewSize;
		f.x = f.y * m_fAspect;
	}
}
