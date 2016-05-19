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
#include "PostProcessing.hlsl"



#define FXAA_PC 1
#define FXAA_HLSL_4 1
#define FXAA_QUALITY__PRESET 12
#define FXAA_GREEN_AS_LUMA 1

#include "Fxaa3_11.h"

float4 ps_fxaa(in PS_INPUT inp) : SV_TARGET
{
    float4 unused = float4(0,0,0,0);

    FxaaFloat2 pos = inp.tex_coord;
    FxaaFloat4 fxaaConsolePosPos = unused;
    FxaaTex tex = { StandardFilter, rt_output };
    FxaaTex fxaaConsole360TexExpBiasNegOne = { StandardFilter, rt_output };
    FxaaTex fxaaConsole360TexExpBiasNegTwo = { StandardFilter, rt_output };

	float w,h;
	rt_output.GetDimensions(w,h);

    FxaaFloat2 fxaaQualityRcpFrame = float2(1.0/w, 1.0/h);

    FxaaFloat4 fxaaConsoleRcpFrameOpt = unused;
    FxaaFloat4 fxaaConsoleRcpFrameOpt2 = unused;
    FxaaFloat4 fxaaConsole360RcpFrameOpt2 = unused;
    FxaaFloat fxaaQualitySubpix = 0.75;
    FxaaFloat fxaaQualityEdgeThreshold = 0.166;
    FxaaFloat fxaaQualityEdgeThresholdMin = 0.0833;
    FxaaFloat fxaaConsoleEdgeSharpness = 8.0;
    FxaaFloat fxaaConsoleEdgeThreshold = 0.125;
    FxaaFloat fxaaConsoleEdgeThresholdMin = 0.05;
    FxaaFloat4 fxaaConsole360ConstDir = unused;

    return FxaaPixelShader(
        pos,
        fxaaConsolePosPos,
        tex,
        fxaaConsole360TexExpBiasNegOne,
        fxaaConsole360TexExpBiasNegTwo,
        fxaaQualityRcpFrame,
        fxaaConsoleRcpFrameOpt,
        fxaaConsoleRcpFrameOpt2,
        fxaaConsole360RcpFrameOpt2,
        fxaaQualitySubpix,
        fxaaQualityEdgeThreshold,
        fxaaQualityEdgeThresholdMin,
        fxaaConsoleEdgeSharpness,
        fxaaConsoleEdgeThreshold,
        fxaaConsoleEdgeThresholdMin,
        fxaaConsole360ConstDir
    );
}
