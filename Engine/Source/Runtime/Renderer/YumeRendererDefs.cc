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

	extern YumeAPIExport const YumeHash VSP_AMBIENTSTARTCOLOR(("AmbientStartColor"));
	extern YumeAPIExport const YumeHash VSP_AMBIENTENDCOLOR(("AmbientEndColor"));
	extern YumeAPIExport const YumeHash VSP_BILLBOARDROT(("BillboardRot"));
	extern YumeAPIExport const YumeHash VSP_CAMERAPOS(("CameraPos"));
	extern YumeAPIExport const YumeHash VSP_CAMERAROT(("CameraRot"));
	extern YumeAPIExport const YumeHash VSP_CLIPPLANE(("ClipPlane"));
	extern YumeAPIExport const YumeHash VSP_NEARCLIP(("NearClip"));
	extern YumeAPIExport const YumeHash VSP_FARCLIP(("FarClip"));
	extern YumeAPIExport const YumeHash VSP_DEPTHMODE(("DepthMode"));
	extern YumeAPIExport const YumeHash VSP_DELTATIME(("DeltaTime"));
	extern YumeAPIExport const YumeHash VSP_ELAPSEDTIME(("ElapsedTime"));
	extern YumeAPIExport const YumeHash VSP_FRUSTUMSIZE(("FrustumSize"));
	extern YumeAPIExport const YumeHash VSP_GBUFFEROFFSETS(("GBufferOffsets"));
	extern YumeAPIExport const YumeHash VSP_LIGHTDIR(("LightDir"));
	extern YumeAPIExport const YumeHash VSP_LIGHTPOS(("LightPos"));
	extern YumeAPIExport const YumeHash VSP_MODEL(("Model"));
	extern YumeAPIExport const YumeHash VSP_VIEW(("View"));
	extern YumeAPIExport const YumeHash VSP_VIEWINV(("ViewInv"));
	extern YumeAPIExport const YumeHash VSP_VIEWPROJ(("ViewProj"));

	extern YumeAPIExport const YumeHash VSP_WORLDVIEW("WorldView");
	extern YumeAPIExport const YumeHash VSP_WORLDINVTRANSPOSEVIEW("WorldInvTransposeView");
	extern YumeAPIExport const YumeHash VSP_WORLDVIEWPROJ("WorldViewProj");

	extern YumeAPIExport const YumeHash VSP_UOFFSET(("UOffset"));
	extern YumeAPIExport const YumeHash VSP_VOFFSET(("VOffset"));
	extern YumeAPIExport const YumeHash VSP_ZONE(("Zone"));
	extern YumeAPIExport const YumeHash VSP_LIGHTMATRICES(("LightMatrices"));
	extern YumeAPIExport const YumeHash VSP_SKINMATRICES(("SkinMatrices"));
	extern YumeAPIExport const YumeHash VSP_VERTEXLIGHTS(("VertexLights"));
	extern YumeAPIExport const YumeHash VSP_FRUSTUMCORNERS(("FrustumCorners"));
	extern YumeAPIExport const YumeHash PSP_OFFSETVECTORS(("OffsetVectors"));
	extern YumeAPIExport const YumeHash PSP_AMBIENTCOLOR(("AmbientColor"));
	extern YumeAPIExport const YumeHash PSP_CAMERAPOS(("CameraPosPS"));
	extern YumeAPIExport const YumeHash PSP_DELTATIME(("DeltaTimePS"));
	extern YumeAPIExport const YumeHash PSP_DEPTHRECONSTRUCT(("DepthReconstruct"));
	extern YumeAPIExport const YumeHash PSP_ELAPSEDTIME(("ElapsedTimePS"));
	extern YumeAPIExport const YumeHash PSP_FOGCOLOR(("FogColor"));
	extern YumeAPIExport const YumeHash PSP_FOGPARAMS(("FogParams"));
	extern YumeAPIExport const YumeHash PSP_GBUFFERINVSIZE(("GBufferInvSize"));
	extern YumeAPIExport const YumeHash PSP_LIGHTCOLOR(("LightColor"));
	extern YumeAPIExport const YumeHash PSP_LIGHTDIR(("LightDirPS"));
	extern YumeAPIExport const YumeHash PSP_LIGHTPOS(("LightPosPS"));
	extern YumeAPIExport const YumeHash PSP_MATDIFFCOLOR(("MatDiffColor"));
	extern YumeAPIExport const YumeHash PSP_MATEMISSIVECOLOR(("MatEmissiveColor"));
	extern YumeAPIExport const YumeHash PSP_MATENVMAPCOLOR(("MatEnvMapColor"));
	extern YumeAPIExport const YumeHash PSP_MATSPECCOLOR(("MatSpecColor"));
	extern YumeAPIExport const YumeHash PSP_NEARCLIP(("NearClipPS"));
	extern YumeAPIExport const YumeHash PSP_FARCLIP(("FarClipPS"));
	extern YumeAPIExport const YumeHash PSP_SHADOWCUBEADJUST(("ShadowCubeAdjust"));
	extern YumeAPIExport const YumeHash PSP_SHADOWDEPTHFADE(("ShadowDepthFade"));
	extern YumeAPIExport const YumeHash PSP_SHADOWINTENSITY(("ShadowIntensity"));
	extern YumeAPIExport const YumeHash PSP_SHADOWMAPINVSIZE(("ShadowMapInvSize"));
	extern YumeAPIExport const YumeHash PSP_SHADOWSPLITS(("ShadowSplits"));
	extern YumeAPIExport const YumeHash PSP_LIGHTMATRICES(("LightMatricesPS"));
	extern YumeAPIExport const YumeHash PSP_VSMSHADOWPARAMS(("VSMShadowParams"));

	extern YumeAPIExport const Vector3 DOT_SCALE(1 / 3.0f,1 / 3.0f,1 / 3.0f);
}
