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
#include "YumeRendererDefs.h"
#include "Core/YumeDefaults.h"

#include "Math/YumeVector3.h"

namespace YumeEngine
{

	extern YumeAPIExport const YumeHash VSP_AMBIENTSTARTCOLOR(GenerateHash("AmbientStartColor"));
	extern YumeAPIExport const YumeHash VSP_AMBIENTENDCOLOR(GenerateHash("AmbientEndColor"));
	extern YumeAPIExport const YumeHash VSP_BILLBOARDROT(GenerateHash("BillboardRot"));
	extern YumeAPIExport const YumeHash VSP_CAMERAPOS(GenerateHash("CameraPos"));
	extern YumeAPIExport const YumeHash VSP_CAMERAROT(GenerateHash("CameraRot"));
	extern YumeAPIExport const YumeHash VSP_CLIPPLANE(GenerateHash("ClipPlane"));
	extern YumeAPIExport const YumeHash VSP_NEARCLIP(GenerateHash("NearClip"));
	extern YumeAPIExport const YumeHash VSP_FARCLIP(GenerateHash("FarClip"));
	extern YumeAPIExport const YumeHash VSP_DEPTHMODE(GenerateHash("DepthMode"));
	extern YumeAPIExport const YumeHash VSP_DELTATIME(GenerateHash("DeltaTime"));
	extern YumeAPIExport const YumeHash VSP_ELAPSEDTIME(GenerateHash("ElapsedTime"));
	extern YumeAPIExport const YumeHash VSP_FRUSTUMSIZE(GenerateHash("FrustumSize"));
	extern YumeAPIExport const YumeHash VSP_GBUFFEROFFSETS(GenerateHash("GBufferOffsets"));
	extern YumeAPIExport const YumeHash VSP_LIGHTDIR(GenerateHash("LightDir"));
	extern YumeAPIExport const YumeHash VSP_LIGHTPOS(GenerateHash("LightPos"));
	extern YumeAPIExport const YumeHash VSP_MODEL(GenerateHash("Model"));
	extern YumeAPIExport const YumeHash VSP_VIEW(GenerateHash("View"));
	extern YumeAPIExport const YumeHash VSP_VIEWINV(GenerateHash("ViewInv"));
	extern YumeAPIExport const YumeHash VSP_VIEWPROJ(GenerateHash("ViewProj"));
	extern YumeAPIExport const YumeHash VSP_UOFFSET(GenerateHash("UOffset"));
	extern YumeAPIExport const YumeHash VSP_VOFFSET(GenerateHash("VOffset"));
	extern YumeAPIExport const YumeHash VSP_ZONE(GenerateHash("Zone"));
	extern YumeAPIExport const YumeHash VSP_LIGHTMATRICES(GenerateHash("LightMatrices"));
	extern YumeAPIExport const YumeHash VSP_SKINMATRICES(GenerateHash("SkinMatrices"));
	extern YumeAPIExport const YumeHash VSP_VERTEXLIGHTS(GenerateHash("VertexLights"));
	extern YumeAPIExport const YumeHash PSP_AMBIENTCOLOR(GenerateHash("AmbientColor"));
	extern YumeAPIExport const YumeHash PSP_CAMERAPOS(GenerateHash("CameraPosPS"));
	extern YumeAPIExport const YumeHash PSP_DELTATIME(GenerateHash("DeltaTimePS"));
	extern YumeAPIExport const YumeHash PSP_DEPTHRECONSTRUCT(GenerateHash("DepthReconstruct"));
	extern YumeAPIExport const YumeHash PSP_ELAPSEDTIME(GenerateHash("ElapsedTimePS"));
	extern YumeAPIExport const YumeHash PSP_FOGCOLOR(GenerateHash("FogColor"));
	extern YumeAPIExport const YumeHash PSP_FOGPARAMS(GenerateHash("FogParams"));
	extern YumeAPIExport const YumeHash PSP_GBUFFERINVSIZE(GenerateHash("GBufferInvSize"));
	extern YumeAPIExport const YumeHash PSP_LIGHTCOLOR(GenerateHash("LightColor"));
	extern YumeAPIExport const YumeHash PSP_LIGHTDIR(GenerateHash("LightDirPS"));
	extern YumeAPIExport const YumeHash PSP_LIGHTPOS(GenerateHash("LightPosPS"));
	extern YumeAPIExport const YumeHash PSP_MATDIFFCOLOR(GenerateHash("MatDiffColor"));
	extern YumeAPIExport const YumeHash PSP_MATEMISSIVECOLOR(GenerateHash("MatEmissiveColor"));
	extern YumeAPIExport const YumeHash PSP_MATENVMAPCOLOR(GenerateHash("MatEnvMapColor"));
	extern YumeAPIExport const YumeHash PSP_MATSPECCOLOR(GenerateHash("MatSpecColor"));
	extern YumeAPIExport const YumeHash PSP_NEARCLIP(GenerateHash("NearClipPS"));
	extern YumeAPIExport const YumeHash PSP_FARCLIP(GenerateHash("FarClipPS"));
	extern YumeAPIExport const YumeHash PSP_SHADOWCUBEADJUST(GenerateHash("ShadowCubeAdjust"));
	extern YumeAPIExport const YumeHash PSP_SHADOWDEPTHFADE(GenerateHash("ShadowDepthFade"));
	extern YumeAPIExport const YumeHash PSP_SHADOWINTENSITY(GenerateHash("ShadowIntensity"));
	extern YumeAPIExport const YumeHash PSP_SHADOWMAPINVSIZE(GenerateHash("ShadowMapInvSize"));
	extern YumeAPIExport const YumeHash PSP_SHADOWSPLITS(GenerateHash("ShadowSplits"));
	extern YumeAPIExport const YumeHash PSP_LIGHTMATRICES(GenerateHash("LightMatricesPS"));
	extern YumeAPIExport const YumeHash PSP_VSMSHADOWPARAMS(GenerateHash("VSMShadowParams"));

	extern YumeAPIExport const Vector3 DOT_SCALE(1 / 3.0f,1 / 3.0f,1 / 3.0f);
}
