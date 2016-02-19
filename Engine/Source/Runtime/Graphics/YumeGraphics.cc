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
// File : YumeGraphics.h
// Date : 8.31.2015
// Comments : 
//
//----------------------------------------------------------------------------

#include "YumeHeaders.h"
#include "YumeGraphics.h"

#include "Math/YumeVector3.h"



#define MakeHash(x) string_hash(x)

namespace YumeEngine
{

	extern YumeAPIExport const YumeStrHash VSP_AMBIENTSTARTCOLOR("AmbientColor");
	extern YumeAPIExport const YumeStrHash VSP_AMBIENTENDCOLOR("AmbientEndColor");
	extern YumeAPIExport const YumeStrHash VSP_BILLBOARDROT("BillboardRot");
	extern YumeAPIExport const YumeStrHash VSP_CAMERAPOS("CameraPos");
	extern YumeAPIExport const YumeStrHash VSP_CAMERAROT("CameraRot");
	extern YumeAPIExport const YumeStrHash VSP_CLIPPLANE("ClipPlane");
	extern YumeAPIExport const YumeStrHash VSP_NEARCLIP("NearClip");
	extern YumeAPIExport const YumeStrHash VSP_FARCLIP("FarClip");
	extern YumeAPIExport const YumeStrHash VSP_DEPTHMODE("DepthMode");
	extern YumeAPIExport const YumeStrHash VSP_DELTATIME("DeltaTime");

	extern YumeAPIExport const YumeStrHash VSP_ELAPSEDTIME("ElapsedTime");
	extern YumeAPIExport const YumeStrHash VSP_FRUSTUMSIZE("FrustumSize");
	extern YumeAPIExport const YumeStrHash VSP_GBUFFEROFFSETS("GBufferOffsets");
	extern YumeAPIExport const YumeStrHash VSP_LIGHTDIR("LightDir");
	extern YumeAPIExport const YumeStrHash VSP_LIGHTPOS("LightPos");
	extern YumeAPIExport const YumeStrHash VSP_MODEL("Model");
	extern YumeAPIExport const YumeStrHash VSP_VIEW("View");
	extern YumeAPIExport const YumeStrHash VSP_VIEWINV("ViewInv");
	extern YumeAPIExport const YumeStrHash VSP_VIEWPROJ("ViewProj");
	extern YumeAPIExport const YumeStrHash VSP_UOFFSET("UOffset");
	extern YumeAPIExport const YumeStrHash VSP_VOFFSET("VOffset");
	extern YumeAPIExport const YumeStrHash VSP_ZONE("Zone");
	extern YumeAPIExport const YumeStrHash VSP_LIGHTMATRICES("LightMatrices");
	extern YumeAPIExport const YumeStrHash VSP_SKINMATRICES("SkinMatrices");
	extern YumeAPIExport const YumeStrHash VSP_VERTEXLIGHTS("VertexLights");
	extern YumeAPIExport const YumeStrHash PSP_AMBIENTCOLOR("AmbientColor");
	extern YumeAPIExport const YumeStrHash PSP_CAMERAPOS("CameraPosPS");
	extern YumeAPIExport const YumeStrHash PSP_DELTATIME("DeltaTimePS");
	extern YumeAPIExport const YumeStrHash PSP_DEPTHRERUCT("DepthReconstruct");
	extern YumeAPIExport const YumeStrHash PSP_ELAPSEDTIME("ElapsedTimePS");
	extern YumeAPIExport const YumeStrHash PSP_FOGCOLOR("FogColor");
	extern YumeAPIExport const YumeStrHash PSP_FOGPARAMS("FogParams");
	extern YumeAPIExport const YumeStrHash PSP_GBUFFERINVSIZE("GBufferInvSize");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTCOLOR("LightColor");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTDIR("LightDirPS");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTPOS("LightPosPS");
	extern YumeAPIExport const YumeStrHash PSP_MATDIFFCOLOR("MatDiffColor");
	extern YumeAPIExport const YumeStrHash PSP_MATEMISSIVECOLOR("MatEmissiveColor");
	extern YumeAPIExport const YumeStrHash PSP_MATENVMAPCOLOR("MatEnvMapColor");
	extern YumeAPIExport const YumeStrHash PSP_MATSPECCOLOR("MatSpecColor");
	extern YumeAPIExport const YumeStrHash PSP_NEARCLIP("NearClipPS");
	extern YumeAPIExport const YumeStrHash PSP_FARCLIP("FarClipPS");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWCUBEADJUST("ShadowCubeAdjust");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWDEPTHFADE("ShadowDepthFade");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWINTENSITY("ShadowIntensity");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWMAPINVSIZE("ShadowMapInvSize");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWSPLITS("ShadowSplits");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTMATRICES("LightMatricesPS");
	extern YumeAPIExport const YumeStrHash PSP_VSMSHADOWPARAMS("VSMShadowParams");

	extern YumeAPIExport  Vector3 DOT_SCALE(1 / 3.0f,1 / 3.0f,1 / 3.0f);
}
