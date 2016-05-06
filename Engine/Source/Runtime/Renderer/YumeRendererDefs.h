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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeRendererDefs_h__
#define __YumeRendererDefs_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeHash.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{


	enum PrimitiveType
	{
		TRIANGLE_LIST = 0,
		LINE_LIST,
		POINT_LIST,
		TRIANGLE_STRIP,
		LINE_STRIP,
		TRIANGLE_FAN
	};

	enum BlendMode
	{
		BLEND_REPLACE = 0,
		BLEND_ADD,
		BLEND_MULTIPLY,
		BLEND_ALPHA,
		BLEND_ADDALPHA,
		BLEND_PREMULALPHA,
		BLEND_INVDESTALPHA,
		BLEND_SUBTRACT,
		BLEND_SUBTRACTALPHA,
		BLEND_MAX,
		MAX_BLENDMODES
	};


	enum CompareMode
	{
		CMP_ALWAYS = 0,
		CMP_EQUAL,
		CMP_NOTEQUAL,
		CMP_LESS,
		CMP_LESSEQUAL,
		CMP_GREATER,
		CMP_GREATEREQUAL,
		MAX_COMPAREMODES
	};


	enum CullMode
	{
		CULL_NONE = 0,
		CULL_CCW,
		CULL_CW,
		MAX_CULLMODES
	};


	enum FillMode
	{
		FILL_SOLID = 0,
		FILL_WIREFRAME,
		FILL_POINT
	};


	enum StencilOp
	{
		OP_KEEP = 0,
		OP_ZERO,
		OP_REF,
		OP_INCR,
		OP_DECR
	};


	enum LockState
	{
		LOCK_NONE = 0,
		LOCK_HARDWARE,
		LOCK_SHADOW,
		LOCK_SCRATCH
	};


	enum VertexElement
	{
		ELEMENT_POSITION = 0,
		ELEMENT_NORMAL,
		ELEMENT_COLOR,
		ELEMENT_TEXCOORD1,
		ELEMENT_TEXCOORD2,
		ELEMENT_TEXCOORD3,
		ELEMENT_CUBETEXCOORD1,
		ELEMENT_CUBETEXCOORD2,
		ELEMENT_TANGENT,
		ELEMENT_BLENDWEIGHTS,
		ELEMENT_BLENDINDICES,
		ELEMENT_INSTANCEMATRIX1,
		ELEMENT_INSTANCEMATRIX2,
		ELEMENT_INSTANCEMATRIX3,
		// Custom 32-bit integer object index. Due to API limitations, not supported on D3D9
		ELEMENT_OBJECTINDEX,
		MAX_VERTEX_ELEMENTS
	};


	enum TextureFilterMode
	{
		FILTER_NEAREST = 0,
		FILTER_BILINEAR,
		FILTER_TRILINEAR,
		FILTER_ANISOTROPIC,
		FILTER_DEFAULT,
		MAX_FILTERMODES
	};


	enum TextureAddressMode
	{
		ADDRESS_WRAP = 0,
		ADDRESS_MIRROR,
		ADDRESS_CLAMP,
		ADDRESS_BORDER,
		MAX_ADDRESSMODES
	};


	enum TextureCoordinate
	{
		COORD_U = 0,
		COORD_V,
		COORD_W,
		MAX_COORDS
	};


	enum TextureUsage
	{
		TEXTURE_STATIC = 0,
		TEXTURE_DYNAMIC,
		TEXTURE_RENDERTARGET,
		TEXTURE_DEPTHSTENCIL,
		TEXTURE_UAV
	};


	enum CubeMapFace
	{
		FACE_POSITIVE_X = 0,
		FACE_NEGATIVE_X,
		FACE_POSITIVE_Y,
		FACE_NEGATIVE_Y,
		FACE_POSITIVE_Z,
		FACE_NEGATIVE_Z,
		MAX_CUBEMAP_FACES
	};


	enum CubeMapLayout
	{
		CML_HORIZONTAL = 0,
		CML_HORIZONTALNVIDIA,
		CML_HORIZONTALCROSS,
		CML_VERTICALCROSS,
		CML_BLENDER
	};


	enum RenderSurfaceUpdateMode
	{
		SURFACE_MANUALUPDATE = 0,
		SURFACE_UPDATEVISIBLE,
		SURFACE_UPDATEALWAYS
	};


	enum ShaderType
	{
		VS = 0,
		PS,
		GS
	};


	enum ShaderParameterGroup
	{
		SP_FRAME = 0,
		SP_CAMERA,
		SP_ZONE,
		SP_LIGHT,
		SP_MATERIAL,
		SP_OBJECT,
		SP_CUSTOM,
		SP_CUSTOM1,
		SP_CUSTOM2,
		MAX_SHADER_PARAMETER_GROUPS
	};


	enum TextureUnit
	{
		TU_DIFFUSE = 0,
		TU_ALBEDOBUFFER = 0,
		TU_NORMAL = 1,
		TU_NORMALBUFFER = 1,
		TU_SPECULAR = 2,
		TU_EMISSIVE = 3,
		TU_ENVIRONMENT = 4,
		TU_VOLUMEMAP = 5,
		TU_CUSTOM1 = 6,
		TU_CUSTOM2 = 7,
		TU_LIGHTRAMP = 8,
		TU_LIGHTSHAPE = 9,
		TU_SHADOWMAP = 10,
		TU_FACESELECT = 11,
		TU_INDIRECTION = 12,
		TU_DEPTHBUFFER = 13,
		TU_LIGHTBUFFER = 14,
		TU_ZONE = 15,
		MAX_MATERIAL_TEXTURE_UNITS = 8,
		MAX_TEXTURE_UNITS = 16
	};

	enum MaterialTextures
	{
		MT_DIFFUSE,
		MT_SPECULAR,
		MT_EMISSIVE,
		MT_NORMAL,
		MT_ALPHA
	};

	enum RenderTargetInOut
	{
		RT_INPUT = 0x1,
		RT_OUTPUT = 0x2,
		RT_DEPTHSTENCIL = 0x4,
		RT_UAV = 0x8
	};

	struct RenderTargetDesc
	{
		unsigned Index;
		unsigned Width;
		unsigned Height;
		unsigned Depth;

		YumeString Name;
		unsigned Format;
		unsigned ArraySize;
		unsigned Mips;

		TextureUsage Usage;
		

		unsigned Type;
		YumeColor ClearColor;
	};

	enum TextureFilterModes
	{
		TFM_Nearest = 0,
		TFM_Bilinear,
		TFM_Trilinear,
		TFM_Aniso,
		TFM_Default
	};

	enum TextureAddressModes
	{
		TAM_Wrap = 0,
		TAM_Mirror,
		TAM_Clamp,
		TAM_Border
	};

	enum TextureComparisonFunctions
	{
		TCF_NEVER,
		TCF_EQUAL,
		TCF_NOTEQUAL,
		TCF_LESS,
		TCF_LESSEQUAL,
		TCF_GREATER,
		TCF_GREATEREQUAL,
		TCF_ALWAYS,
	};

	struct SamplerStateDesc
	{
		TextureFilterModes Filter;
		TextureAddressModes AddressModeU;
		TextureAddressModes AddressModeV;
		TextureAddressModes AddressModeW;

		TextureComparisonFunctions ComparisonFunc;

		float MaxLOD;
		unsigned MaxAniso;

		YumeString Name;

		float BorderColor[4];

		unsigned Register;
		ShaderType Type;
	};

	enum MiscRenderingFlags
	{
		RF_NODEPTHSTENCIL = 0x1,
		RF_NOBLENDSTATE = 0x2,
		RF_CLEAR = 0x4
	};

	struct GIParameters
	{
		float Scale;
		float LPVFlux;
		bool DebugView;
	};

	extern YumeAPIExport const YumeHash VSP_AMBIENTSTARTCOLOR;
	extern YumeAPIExport const YumeHash VSP_AMBIENTENDCOLOR;
	extern YumeAPIExport const YumeHash VSP_BILLBOARDROT;
	extern YumeAPIExport const YumeHash VSP_CAMERAPOS;
	extern YumeAPIExport const YumeHash VSP_CAMERAROT;
	extern YumeAPIExport const YumeHash VSP_CLIPPLANE;
	extern YumeAPIExport const YumeHash VSP_NEARCLIP;
	extern YumeAPIExport const YumeHash VSP_FARCLIP;
	extern YumeAPIExport const YumeHash VSP_DEPTHMODE;
	extern YumeAPIExport const YumeHash VSP_DELTATIME;
	extern YumeAPIExport const YumeHash VSP_ELAPSEDTIME;
	extern YumeAPIExport const YumeHash VSP_FRUSTUMSIZE;
	extern YumeAPIExport const YumeHash VSP_GBUFFEROFFSETS;
	extern YumeAPIExport const YumeHash VSP_LIGHTDIR;
	extern YumeAPIExport const YumeHash VSP_LIGHTPOS;
	extern YumeAPIExport const YumeHash VSP_MODEL;
	extern YumeAPIExport const YumeHash VSP_VIEW;
	extern YumeAPIExport const YumeHash VSP_VIEWINV;
	extern YumeAPIExport const YumeHash VSP_VIEWPROJ;

	extern YumeAPIExport const YumeHash VSP_WORLDVIEW;
	extern YumeAPIExport const YumeHash VSP_WORLDINVTRANSPOSEVIEW;
	extern YumeAPIExport const YumeHash VSP_WORLDVIEWPROJ;

	extern YumeAPIExport const YumeHash VSP_UOFFSET;
	extern YumeAPIExport const YumeHash VSP_VOFFSET;
	extern YumeAPIExport const YumeHash VSP_ZONE;
	extern YumeAPIExport const YumeHash VSP_LIGHTMATRICES;
	extern YumeAPIExport const YumeHash VSP_SKINMATRICES;
	extern YumeAPIExport const YumeHash VSP_VERTEXLIGHTS;
	extern YumeAPIExport const YumeHash VSP_FRUSTUMCORNERS;
	extern YumeAPIExport const YumeHash PSP_OFFSETVECTORS;
	extern YumeAPIExport const YumeHash PSP_AMBIENTCOLOR;
	extern YumeAPIExport const YumeHash PSP_CAMERAPOS;
	extern YumeAPIExport const YumeHash PSP_DELTATIME;
	extern YumeAPIExport const YumeHash PSP_DEPTHRECONSTRUCT;
	extern YumeAPIExport const YumeHash PSP_ELAPSEDTIME;
	extern YumeAPIExport const YumeHash PSP_FOGCOLOR;
	extern YumeAPIExport const YumeHash PSP_FOGPARAMS;
	extern YumeAPIExport const YumeHash PSP_GBUFFERINVSIZE;
	extern YumeAPIExport const YumeHash PSP_LIGHTCOLOR;
	extern YumeAPIExport const YumeHash PSP_LIGHTDIR;
	extern YumeAPIExport const YumeHash PSP_LIGHTPOS;
	extern YumeAPIExport const YumeHash PSP_LIGHTPOSSCREEN;
	extern YumeAPIExport const YumeHash PSP_MATDIFFCOLOR;
	extern YumeAPIExport const YumeHash PSP_MATEMISSIVECOLOR;
	extern YumeAPIExport const YumeHash PSP_MATENVMAPCOLOR;
	extern YumeAPIExport const YumeHash PSP_MATSPECCOLOR;
	extern YumeAPIExport const YumeHash PSP_NEARCLIP;
	extern YumeAPIExport const YumeHash PSP_FARCLIP;
	extern YumeAPIExport const YumeHash PSP_SHADOWCUBEADJUST;
	extern YumeAPIExport const YumeHash PSP_SHADOWDEPTHFADE;
	extern YumeAPIExport const YumeHash PSP_SHADOWINTENSITY;
	extern YumeAPIExport const YumeHash PSP_SHADOWMAPINVSIZE;
	extern YumeAPIExport const YumeHash PSP_SHADOWSPLITS;
	extern YumeAPIExport const YumeHash PSP_LIGHTMATRICES;
	extern YumeAPIExport const YumeHash PSP_VSMSHADOWPARAMS;

	// Scale calculation from bounding box diagonal.
	extern YumeAPIExport const Vector3 DOT_SCALE;

	static const int QUALITY_LOW = 0;
	static const int QUALITY_MEDIUM = 1;
	static const int QUALITY_HIGH = 2;
	static const int QUALITY_MAX = 15;

	static const unsigned CLEAR_COLOR = 0x1;
	static const unsigned CLEAR_DEPTH = 0x2;
	static const unsigned CLEAR_STENCIL = 0x4;

	static const unsigned MASK_NONE = 0x0;
	static const unsigned MASK_POSITION = 0x1;
	static const unsigned MASK_NORMAL = 0x2;
	static const unsigned MASK_COLOR = 0x4;
	static const unsigned MASK_TEXCOORD1 = 0x8;
	static const unsigned MASK_TEXCOORD2 = 0x10;
	static const unsigned MASK_CUBETEXCOORD1 = 0x20;
	static const unsigned MASK_CUBETEXCOORD2 = 0x40;
	static const unsigned MASK_TANGENT = 0x80;
	static const unsigned MASK_BLENDWEIGHTS = 0x100;
	static const unsigned MASK_BLENDINDICES = 0x200;
	static const unsigned MASK_INSTANCEMATRIX1 = 0x400;
	static const unsigned MASK_INSTANCEMATRIX2 = 0x800;
	static const unsigned MASK_INSTANCEMATRIX3 = 0x1000;
	static const unsigned MASK_OBJECTINDEX = 0x2000;
	static const unsigned MASK_TEXCOORD3 = 0x4000;
	static const unsigned MASK_DEFAULT = 0xffffffff;
	static const unsigned NO_ELEMENT = 0xffffffff;

	static const int MAX_RENDERTARGETS = 6;
	static const int MAX_VERTEX_STREAMS = 4;
	static const int MAX_CONSTANT_REGISTERS = 256;

	static const int BITS_PER_COMPONENT = 8;
}


//----------------------------------------------------------------------------
#endif
