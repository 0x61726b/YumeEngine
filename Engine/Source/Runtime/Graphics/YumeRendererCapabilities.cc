///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 arkenthera

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeRendererCapabilities.cpp
/// Date : 9.1.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeHeaders.h"
#include "YumeRendererCapabilities.h"

#include "Core/YumeStringConverter.h"

#include "Logging/logging.h"

namespace YumeEngine
{
	//-----------------------------------------------------------------------
	YumeRendererCapabilities::YumeRendererCapabilities()
		: mVendor(GPU_UNKNOWN)
		, mNumWorldMatrices(0)
		, mNumTextureUnits(0)
		, mStencilBufferBitDepth(0)
		, mNumVertexBlendMatrices(0)
		, mNumMultiRenderTargets(1)
		, mNonPOW2TexturesLimited(false)
	{

		for (int i = 0; i < CAPS_CATEGORY_COUNT; i++)
		{
			mCapabilities[i] = 0;
		}
		mCategoryRelevant[CAPS_CATEGORY_COMMON] = true;
		mCategoryRelevant[CAPS_CATEGORY_COMMON_2] = true;
		// each rendersystem should enable these
		mCategoryRelevant[CAPS_CATEGORY_D3D9] = false;
		mCategoryRelevant[CAPS_CATEGORY_GL] = false;


	}
	//-----------------------------------------------------------------------
	YumeRendererCapabilities::~YumeRendererCapabilities()
	{
	}
	//-----------------------------------------------------------------------
	void YumeRendererCapabilities::Log()
	{
		YUMELOG_DEBUG("Renderer Capabilities");
		YUMELOG_DEBUG("-------------------------");
		YUMELOG_DEBUG("RenderSystem Name: " + getRenderSystemName());
		YUMELOG_DEBUG("GPU Vendor: " + vendorToString(getVendor()));
		YUMELOG_DEBUG("Device Name: " + getDeviceName());
		YUMELOG_DEBUG("Driver Version: " + getDriverVersion().toString());
		YUMELOG_DEBUG(" * Fixed function pipeline: "
			+ YumeStringConverter::toString(hasCapability(RSC_FIXED_FUNCTION), true));
		YUMELOG_DEBUG(
			" * Hardware generation of mipmaps: "
			+ YumeStringConverter::toString(hasCapability(RSC_AUTOMIPMAP), true));
		YUMELOG_DEBUG(
			" * Texture blending: "
			+ YumeStringConverter::toString(hasCapability(RSC_BLENDING), true));
		YUMELOG_DEBUG(
			" * Anisotropic texture filtering: "
			+ YumeStringConverter::toString(hasCapability(RSC_ANISOTROPY), true));
		YUMELOG_DEBUG(
			" * Dot product texture operation: "
			+ YumeStringConverter::toString(hasCapability(RSC_DOT3), true));
		YUMELOG_DEBUG(
			" * Cube mapping: "
			+ YumeStringConverter::toString(hasCapability(RSC_CUBEMAPPING), true));
		YUMELOG_DEBUG(
			" * Hardware stencil buffer: "
			+ YumeStringConverter::toString(hasCapability(RSC_HWSTENCIL), true));
		if (hasCapability(RSC_HWSTENCIL))
		{
			YUMELOG_DEBUG(
				"   - Stencil depth: "
				+ YumeStringConverter::toString(getStencilBufferBitDepth()));
			YUMELOG_DEBUG(
				"   - Two sided stencil support: "
				+ YumeStringConverter::toString(hasCapability(RSC_TWO_SIDED_STENCIL), true));
			YUMELOG_DEBUG(
				"   - Wrap stencil values: "
				+ YumeStringConverter::toString(hasCapability(RSC_STENCIL_WRAP), true));
		}
		YUMELOG_DEBUG(
			" * Hardware vertex / index buffers: "
			+ YumeStringConverter::toString(hasCapability(RSC_VBO), true));
		YUMELOG_DEBUG(
			" * Vertex programs: "
			+ YumeStringConverter::toString(hasCapability(RSC_VERTEX_PROGRAM), true));
		YUMELOG_DEBUG(
			" * Number of floating-point constants for vertex programs: "
			+ YumeStringConverter::toString(mVertexProgramConstantFloatCount));
		YUMELOG_DEBUG(
			" * Number of integer constants for vertex programs: "
			+ YumeStringConverter::toString(mVertexProgramConstantIntCount));
		YUMELOG_DEBUG(
			" * Number of boolean constants for vertex programs: "
			+ YumeStringConverter::toString(mVertexProgramConstantBoolCount));
		YUMELOG_DEBUG(
			" * Fragment programs: "
			+ YumeStringConverter::toString(hasCapability(RSC_FRAGMENT_PROGRAM), true));
		YUMELOG_DEBUG(
			" * Number of floating-point constants for fragment programs: "
			+ YumeStringConverter::toString(mFragmentProgramConstantFloatCount));
		YUMELOG_DEBUG(
			" * Number of integer constants for fragment programs: "
			+ YumeStringConverter::toString(mFragmentProgramConstantIntCount));
		YUMELOG_DEBUG(
			" * Number of boolean constants for fragment programs: "
			+ YumeStringConverter::toString(mFragmentProgramConstantBoolCount));
		YUMELOG_DEBUG(
			" * Geometry programs: "
			+ YumeStringConverter::toString(hasCapability(RSC_GEOMETRY_PROGRAM), true));
		YUMELOG_DEBUG(
			" * Number of floating-point constants for geometry programs: "
			+ YumeStringConverter::toString(mGeometryProgramConstantFloatCount));
		YUMELOG_DEBUG(
			" * Number of integer constants for geometry programs: "
			+ YumeStringConverter::toString(mGeometryProgramConstantIntCount));
		YUMELOG_DEBUG(
			" * Number of boolean constants for geometry programs: "
			+ YumeStringConverter::toString(mGeometryProgramConstantBoolCount));
		YumeString profileList = "";
		for (ShaderProfiles::iterator iter = mSupportedShaderProfiles.begin(), end = mSupportedShaderProfiles.end();
		iter != end; ++iter)
		{
			profileList += " " + *iter;
		}
		YUMELOG_DEBUG(" * Supported Shader Profiles:" + profileList);

		YUMELOG_DEBUG(
			" * Texture Compression: "
			+ YumeStringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION), true));
		if (hasCapability(RSC_TEXTURE_COMPRESSION))
		{
			YUMELOG_DEBUG(
				"   - DXT: "
				+ YumeStringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_DXT), true));
			YUMELOG_DEBUG(
				"   - VTC: "
				+ YumeStringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_VTC), true));
			YUMELOG_DEBUG(
				"   - PVRTC: "
				+ YumeStringConverter::toString(hasCapability(RSC_TEXTURE_COMPRESSION_PVRTC), true));
		}

		YUMELOG_DEBUG(
			" * Scissor Rectangle: "
			+ YumeStringConverter::toString(hasCapability(RSC_SCISSOR_TEST), true));
		YUMELOG_DEBUG(
			" * Hardware Occlusion Query: "
			+ YumeStringConverter::toString(hasCapability(RSC_HWOCCLUSION), true));
		YUMELOG_DEBUG(
			" * User clip planes: "
			+ YumeStringConverter::toString(hasCapability(RSC_USER_CLIP_PLANES), true));
		YUMELOG_DEBUG(
			" * VET_UBYTE4 vertex element type: "
			+ YumeStringConverter::toString(hasCapability(RSC_VERTEX_FORMAT_UBYTE4), true));
		YUMELOG_DEBUG(
			" * Infinite far plane projection: "
			+ YumeStringConverter::toString(hasCapability(RSC_INFINITE_FAR_PLANE), true));
		YUMELOG_DEBUG(
			" * Hardware render-to-texture: "
			+ YumeStringConverter::toString(hasCapability(RSC_HWRENDER_TO_TEXTURE), true));
		YUMELOG_DEBUG(
			" * Floating point textures: "
			+ YumeStringConverter::toString(hasCapability(RSC_TEXTURE_FLOAT), true));
		YUMELOG_DEBUG(
			" * Non-power-of-two textures: "
			+ YumeStringConverter::toString(hasCapability(RSC_NON_POWER_OF_2_TEXTURES), true)
			+ (mNonPOW2TexturesLimited ? " (limited)" : ""));
		YUMELOG_DEBUG(
			" * Volume textures: "
			+ YumeStringConverter::toString(hasCapability(RSC_TEXTURE_3D), true));
		YUMELOG_DEBUG(
			" * Multiple Render Targets: "
			+ YumeStringConverter::toString(mNumMultiRenderTargets));
		YUMELOG_DEBUG(
			"   - With different bit depths: " + YumeStringConverter::toString(hasCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS), true));
		YUMELOG_DEBUG(
			" * Point Sprites: "
			+ YumeStringConverter::toString(hasCapability(RSC_POINT_SPRITES), true));
		YUMELOG_DEBUG(
			" * Extended point parameters: "
			+ YumeStringConverter::toString(hasCapability(RSC_POINT_EXTENDED_PARAMETERS), true));
		if (hasCapability(RSC_POINT_SPRITES))
		{
			YUMELOG_DEBUG(
				" * Max Point Size: "
				+ YumeStringConverter::toString((int)mMaxPointSize));
		}
		YUMELOG_DEBUG(
			" * Vertex texture fetch: "
			+ YumeStringConverter::toString(hasCapability(RSC_VERTEX_TEXTURE_FETCH), true));
		YUMELOG_DEBUG(
			" * Number of world matrices: "
			+ YumeStringConverter::toString(mNumWorldMatrices));
		YUMELOG_DEBUG(
			" * Number of texture units: "
			+ YumeStringConverter::toString(mNumTextureUnits));
		YUMELOG_DEBUG(
			" * Stencil buffer depth: "
			+ YumeStringConverter::toString(mStencilBufferBitDepth));
		YUMELOG_DEBUG(
			" * Number of vertex blend matrices: "
			+ YumeStringConverter::toString(mNumVertexBlendMatrices));
		if (hasCapability(RSC_VERTEX_TEXTURE_FETCH))
		{
			YUMELOG_DEBUG(
				"   - Max vertex textures: "
				+ YumeStringConverter::toString(mNumVertexTextureUnits));
			YUMELOG_DEBUG(
				"   - Vertex textures shared: "
				+ YumeStringConverter::toString(mVertexTextureUnitsShared, true));

		}
		YUMELOG_DEBUG(
			" * Render to Vertex Buffer : "
			+ YumeStringConverter::toString(hasCapability(RSC_HWRENDER_TO_VERTEX_BUFFER), true));

		if (mCategoryRelevant[CAPS_CATEGORY_GL])
		{
			YUMELOG_DEBUG(
				" * GL 1.5 without VBO workaround: "
				+ YumeStringConverter::toString(hasCapability(RSC_GL1_5_NOVBO), true));

			YUMELOG_DEBUG(
				" * Frame Buffer objects: "
				+ YumeStringConverter::toString(hasCapability(RSC_FBO), true));
			YUMELOG_DEBUG(
				" * Frame Buffer objects (ARB extension): "
				+ YumeStringConverter::toString(hasCapability(RSC_FBO_ARB), true));
			YUMELOG_DEBUG(
				" * Frame Buffer objects (ATI extension): "
				+ YumeStringConverter::toString(hasCapability(RSC_FBO_ATI), true));
			YUMELOG_DEBUG(
				" * PBuffer support: "
				+ YumeStringConverter::toString(hasCapability(RSC_PBUFFER), true));
			YUMELOG_DEBUG(
				" * GL 1.5 without HW-occlusion workaround: "
				+ YumeStringConverter::toString(hasCapability(RSC_GL1_5_NOHWOCCLUSION), true));
			YUMELOG_DEBUG(
				" * Separate shader objects: "
				+ YumeStringConverter::toString(hasCapability(RSC_SEPARATE_SHADER_OBJECTS), true));
		}

		if (mCategoryRelevant[CAPS_CATEGORY_D3D9])
		{
			YUMELOG_DEBUG(
				" * DirectX per stage constants: "
				+ YumeStringConverter::toString(hasCapability(RSC_PERSTAGECONSTANT), true));
		}

	}
	//---------------------------------------------------------------------
	StringVector YumeRendererCapabilities::msGPUVendorStrings;
	//---------------------------------------------------------------------
	GPUVendor YumeRendererCapabilities::vendorFromString(const YumeString& vendorString)
	{
		initVendorStrings();
		GPUVendor ret = GPU_UNKNOWN;
		YumeString cmpString = vendorString;
		StringUtil::toLowerCase(cmpString);
		for (int i = 0; i < GPU_VENDOR_COUNT; ++i)
		{
			// case insensitive (lower case)
			if (msGPUVendorStrings[i] == cmpString)
			{
				ret = static_cast<GPUVendor>(i);
				break;
			}
		}

		return ret;

	}
	//---------------------------------------------------------------------
	YumeString YumeRendererCapabilities::vendorToString(GPUVendor v)
	{
		initVendorStrings();
		return msGPUVendorStrings[v];
	}
	//---------------------------------------------------------------------
	void YumeRendererCapabilities::initVendorStrings()
	{
		if (msGPUVendorStrings.empty())
		{
			// Always lower case!
			msGPUVendorStrings.resize(GPU_VENDOR_COUNT);
			msGPUVendorStrings[GPU_UNKNOWN] = "unknown";
			msGPUVendorStrings[GPU_NVIDIA] = "nvidia";
			msGPUVendorStrings[GPU_ATI] = "ati";
			msGPUVendorStrings[GPU_INTEL] = "intel";
			msGPUVendorStrings[GPU_3DLABS] = "3dlabs";
			msGPUVendorStrings[GPU_S3] = "s3";
			msGPUVendorStrings[GPU_MATROX] = "matrox";
			msGPUVendorStrings[GPU_SIS] = "sis";
			msGPUVendorStrings[GPU_IMAGINATION_TECHNOLOGIES] = "imagination technologies";
			msGPUVendorStrings[GPU_APPLE] = "apple";    // iOS Simulator
		}
	}

}