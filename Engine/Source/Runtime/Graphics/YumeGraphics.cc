//----------------------------------------------------------------------------
//Chiika Api
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

#include <boost/functional/hash.hpp>
#include "Math/YumeVector3.h"



#define MakeHash(x) string_hash(x)

namespace YumeEngine
{
	boost::hash<YumeString> string_hash;	

	extern YumeAPIExport const YumeStrHash VSP_AMBIENTSTARTCOLOR = MakeHash("AmbientColor");
	extern YumeAPIExport const YumeStrHash VSP_AMBIENTENDCOLOR = MakeHash("AmbientEndColor");
	extern YumeAPIExport const YumeStrHash VSP_BILLBOARDROT = MakeHash("BillboardRot");
	extern YumeAPIExport const YumeStrHash VSP_CAMERAPOS = MakeHash("CameraPos");
	extern YumeAPIExport const YumeStrHash VSP_CAMERAROT = MakeHash("CameraRot");
	extern YumeAPIExport const YumeStrHash VSP_CLIPPLANE = MakeHash("ClipPlane");
	extern YumeAPIExport const YumeStrHash VSP_NEARCLIP = MakeHash("NearClip");
	extern YumeAPIExport const YumeStrHash VSP_FARCLIP = MakeHash("FarClip");
	extern YumeAPIExport const YumeStrHash VSP_DEPTHMODE = MakeHash("DepthMode");
	extern YumeAPIExport const YumeStrHash VSP_DELTATIME = MakeHash("DeltaTime");

	extern YumeAPIExport const YumeStrHash VSP_ELAPSEDTIME = MakeHash("ElapsedTime");
	extern YumeAPIExport const YumeStrHash VSP_FRUSTUMSIZE = MakeHash("FrustumSize");
	extern YumeAPIExport const YumeStrHash VSP_GBUFFEROFFSETS = MakeHash("GBufferOffsets");
	extern YumeAPIExport const YumeStrHash VSP_LIGHTDIR = MakeHash("LightDir");
	extern YumeAPIExport const YumeStrHash VSP_LIGHTPOS = MakeHash("LightPos");
	extern YumeAPIExport const YumeStrHash VSP_MODEL = MakeHash("Model");
	extern YumeAPIExport const YumeStrHash VSP_VIEW = MakeHash("View");
	extern YumeAPIExport const YumeStrHash VSP_VIEWINV = MakeHash("ViewInv");
	extern YumeAPIExport const YumeStrHash VSP_VIEWPROJ = MakeHash("ViewProj");
	extern YumeAPIExport const YumeStrHash VSP_UOFFSET = MakeHash("UOffset");
	extern YumeAPIExport const YumeStrHash VSP_VOFFSET = MakeHash("VOffset");
	extern YumeAPIExport const YumeStrHash VSP_ZONE = MakeHash("Zone");
	extern YumeAPIExport const YumeStrHash VSP_LIGHTMATRICES = MakeHash("LightMatrices");
	extern YumeAPIExport const YumeStrHash VSP_SKINMATRICES = MakeHash("SkinMatrices");
	extern YumeAPIExport const YumeStrHash VSP_VERTEXLIGHTS = MakeHash("VertexLights");
	extern YumeAPIExport const YumeStrHash PSP_AMBIENTCOLOR = MakeHash("AmbientColor");
	extern YumeAPIExport const YumeStrHash PSP_CAMERAPOS = MakeHash("CameraPosPS");
	extern YumeAPIExport const YumeStrHash PSP_DELTATIME = MakeHash("DeltaTimePS");
	extern YumeAPIExport const YumeStrHash PSP_DEPTHRERUCT = MakeHash("DepthReconstruct");
	extern YumeAPIExport const YumeStrHash PSP_ELAPSEDTIME = MakeHash("ElapsedTimePS");
	extern YumeAPIExport const YumeStrHash PSP_FOGCOLOR = MakeHash("FogColor");
	extern YumeAPIExport const YumeStrHash PSP_FOGPARAMS = MakeHash("FogParams");
	extern YumeAPIExport const YumeStrHash PSP_GBUFFERINVSIZE = MakeHash("GBufferInvSize");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTCOLOR = MakeHash("LightColor");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTDIR = MakeHash("LightDirPS");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTPOS = MakeHash("LightPosPS");
	extern YumeAPIExport const YumeStrHash PSP_MATDIFFCOLOR = MakeHash("MatDiffColor");
	extern YumeAPIExport const YumeStrHash PSP_MATEMISSIVECOLOR = MakeHash("MatEmissiveColor");
	extern YumeAPIExport const YumeStrHash PSP_MATENVMAPCOLOR = MakeHash("MatEnvMapColor");
	extern YumeAPIExport const YumeStrHash PSP_MATSPECCOLOR = MakeHash("MatSpecColor");
	extern YumeAPIExport const YumeStrHash PSP_NEARCLIP = MakeHash("NearClipPS");
	extern YumeAPIExport const YumeStrHash PSP_FARCLIP = MakeHash("FarClipPS");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWCUBEADJUST = MakeHash("ShadowCubeAdjust");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWDEPTHFADE = MakeHash("ShadowDepthFade");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWINTENSITY = MakeHash("ShadowIntensity");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWMAPINVSIZE = MakeHash("ShadowMapInvSize");
	extern YumeAPIExport const YumeStrHash PSP_SHADOWSPLITS = MakeHash("ShadowSplits");
	extern YumeAPIExport const YumeStrHash PSP_LIGHTMATRICES = MakeHash("LightMatricesPS");
	extern YumeAPIExport const YumeStrHash PSP_VSMSHADOWPARAMS = MakeHash("VSMShadowParams");

	extern YumeAPIExport  Vector3 DOT_SCALE(1 / 3.0f,1 / 3.0f,1 / 3.0f);
}
