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
/// File : YumeRenderer.h
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumeGraphics_h__
#define __YumeGraphics_h__
//---------------------------------------------------------------------------------
#include "Core/YumeRequired.h"
#include <boost/functional/hash.hpp>
//---------------------------------------------------------------------------------
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

	enum GeometryType
	{
		GEOM_STATIC = 0,
		GEOM_SKINNED = 1,
		GEOM_INSTANCED = 2,
		GEOM_BILLBOARD = 3,
		GEOM_DIRBILLBOARD = 4,
		GEOM_STATIC_NOINSTANCING = 5,
		MAX_GEOMETRYTYPES = 5,
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
		TEXTURE_DEPTHSTENCIL
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
		MAX_SHADER_PARAMETER_GROUPS
	};

	/// Texture units.
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

	/// Billboard camera facing modes.
	enum FaceCameraMode
	{
		FC_NONE = 0,
		FC_ROTATE_XYZ,
		FC_ROTATE_Y,
		FC_LOOKAT_XYZ,
		FC_LOOKAT_Y,
		FC_DIRECTION
	};

	/// Shadow type
	enum ShadowQuality
	{
		SHADOWQUALITY_SIMPLE_16BIT = 0,
		SHADOWQUALITY_SIMPLE_24BIT,
		SHADOWQUALITY_PCF_16BIT,
		SHADOWQUALITY_PCF_24BIT,
		SHADOWQUALITY_VSM,
		SHADOWQUALITY_BLUR_VSM
	};


	// Inbuilt shader parameters.
	extern YumeAPIExport const YumeStrHash VSP_AMBIENTSTARTCOLOR;
	extern YumeAPIExport const YumeStrHash VSP_AMBIENTENDCOLOR;
	extern YumeAPIExport const YumeStrHash VSP_BILLBOARDROT;
	extern YumeAPIExport const YumeStrHash VSP_CAMERAPOS;
	extern YumeAPIExport const YumeStrHash VSP_CAMERAROT;
	extern YumeAPIExport const YumeStrHash VSP_CLIPPLANE;
	extern YumeAPIExport const YumeStrHash VSP_NEARCLIP;
	extern YumeAPIExport const YumeStrHash VSP_FARCLIP;
	extern YumeAPIExport const YumeStrHash VSP_DEPTHMODE;
	extern YumeAPIExport const YumeStrHash VSP_DELTATIME;
	extern YumeAPIExport const YumeStrHash VSP_ELAPSEDTIME;
	extern YumeAPIExport const YumeStrHash VSP_FRUSTUMSIZE;
	extern YumeAPIExport const YumeStrHash VSP_GBUFFEROFFSETS;
	extern YumeAPIExport const YumeStrHash VSP_LIGHTDIR;
	extern YumeAPIExport const YumeStrHash VSP_LIGHTPOS;
	extern YumeAPIExport const YumeStrHash VSP_MODEL;
	extern YumeAPIExport const YumeStrHash VSP_VIEW;
	extern YumeAPIExport const YumeStrHash VSP_VIEWINV;
	extern YumeAPIExport const YumeStrHash VSP_VIEWPROJ;
	extern YumeAPIExport const YumeStrHash VSP_UOFFSET;
	extern YumeAPIExport const YumeStrHash VSP_VOFFSET;
	extern YumeAPIExport const YumeStrHash VSP_ZONE;
	extern YumeAPIExport const YumeStrHash VSP_LIGHTMATRICES;
	extern YumeAPIExport const YumeStrHash VSP_SKINMATRICES;
	extern YumeAPIExport const YumeStrHash VSP_VERTEXLIGHTS;
	extern YumeAPIExport const YumeStrHash PSP_AMBIENTCOLOR;
	extern YumeAPIExport const YumeStrHash PSP_CAMERAPOS;
	extern YumeAPIExport const YumeStrHash PSP_DELTATIME;
	extern YumeAPIExport const YumeStrHash PSP_DEPTHRERUCT;
	extern YumeAPIExport const YumeStrHash PSP_ELAPSEDTIME;
	extern YumeAPIExport const YumeStrHash PSP_FOGCOLOR;
	extern YumeAPIExport const YumeStrHash PSP_FOGPARAMS;
	extern YumeAPIExport const YumeStrHash PSP_GBUFFERINVSIZE;
	extern YumeAPIExport const YumeStrHash PSP_LIGHTCOLOR;
	extern YumeAPIExport const YumeStrHash PSP_LIGHTDIR;
	extern YumeAPIExport const YumeStrHash PSP_LIGHTPOS;
	extern YumeAPIExport const YumeStrHash PSP_MATDIFFCOLOR;
	extern YumeAPIExport const YumeStrHash PSP_MATEMISSIVECOLOR;
	extern YumeAPIExport const YumeStrHash PSP_MATENVMAPCOLOR;
	extern YumeAPIExport const YumeStrHash PSP_MATSPECCOLOR;
	extern YumeAPIExport const YumeStrHash PSP_NEARCLIP;
	extern YumeAPIExport const YumeStrHash PSP_FARCLIP;
	extern YumeAPIExport const YumeStrHash PSP_SHADOWCUBEADJUST;
	extern YumeAPIExport const YumeStrHash PSP_SHADOWDEPTHFADE;
	extern YumeAPIExport const YumeStrHash PSP_SHADOWINTENSITY;
	extern YumeAPIExport const YumeStrHash PSP_SHADOWMAPINVSIZE;
	extern YumeAPIExport const YumeStrHash PSP_SHADOWSPLITS;
	extern YumeAPIExport const YumeStrHash PSP_LIGHTMATRICES;
	extern YumeAPIExport const YumeStrHash PSP_VSMSHADOWPARAMS;

	// Scale calculation from bounding box diagonal.
	extern YumeAPIExport  Vector3 DOT_SCALE;

	static  int QUALITY_LOW = 0;
	static  int QUALITY_MEDIUM = 1;
	static  int QUALITY_HIGH = 2;
	static  int QUALITY_MAX = 15;

	static  unsigned CLEAR_COLOR = 0x1;
	static  unsigned CLEAR_DEPTH = 0x2;
	static  unsigned CLEAR_STENCIL = 0x4;

	static  unsigned MASK_NONE = 0x0;
	static  unsigned MASK_POSITION = 0x1;
	static  unsigned MASK_NORMAL = 0x2;
	static  unsigned MASK_COLOR = 0x4;
	static  unsigned MASK_TEXCOORD1 = 0x8;
	static  unsigned MASK_TEXCOORD2 = 0x10;
	static  unsigned MASK_CUBETEXCOORD1 = 0x20;
	static  unsigned MASK_CUBETEXCOORD2 = 0x40;
	static  unsigned MASK_TANGENT = 0x80;
	static  unsigned MASK_BLENDWEIGHTS = 0x100;
	static  unsigned MASK_BLENDINDICES = 0x200;
	static  unsigned MASK_INSTANCEMATRIX1 = 0x400;
	static  unsigned MASK_INSTANCEMATRIX2 = 0x800;
	static  unsigned MASK_INSTANCEMATRIX3 = 0x1000;
	static  unsigned MASK_OBJECTINDEX = 0x2000;
	static  unsigned MASK_DEFAULT = 0xffffffff;
	static  unsigned NO_ELEMENT = 0xffffffff;

	static  int MAX_RENDERTARGETS = 4;
	static  int MAX_VERTEX_STREAMS = 4;
	static  int MAX_ANT_REGISTERS = 256;

	static  int BITS_PER_COMPONENT = 8;
}



//---------------------------------------------------------------------------------
#endif