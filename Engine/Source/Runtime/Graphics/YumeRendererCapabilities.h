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
/// File : YumeRendererCapabilities.h
/// Date : 9.1.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeRendererCapabilities_h__
#define __YumeRendererCapabilities_h__
//---------------------------------------------------------------------------------
#include "Core/YumeRequired.h"
#include "Core/YumeString.h"
#include "Core/YumeStringConverter.h"
#include "Core/YumeStringVector.h"

#define CAPS_CATEGORY_SIZE 4
#define YUME_CAPS_BITSHIFT (32 - CAPS_CATEGORY_SIZE)
#define CAPS_CATEGORY_MASK (((1 << CAPS_CATEGORY_SIZE) - 1) << YUME_CAPS_BITSHIFT)
#define YUME_CAPS_VALUE(cat, val) ((cat << YUME_CAPS_BITSHIFT) | (1 << val))


namespace YumeEngine
{
	enum CapabilitiesCategory
	{
		CAPS_CATEGORY_COMMON = 0,
		CAPS_CATEGORY_COMMON_2 = 1,
		CAPS_CATEGORY_D3D9 = 2,
		CAPS_CATEGORY_GL = 3,
		/// Placeholder for max value
		CAPS_CATEGORY_COUNT = 4
	};

	enum Capabilities
	{
		/// Supports generating mipmaps in hardware
		RSC_AUTOMIPMAP = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 0),
		RSC_BLENDING = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 1),
		/// Supports anisotropic texture filtering
		RSC_ANISOTROPY = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 2),
		/// Supports fixed-function DOT3 texture blend
		RSC_DOT3 = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 3),
		/// Supports cube mapping
		RSC_CUBEMAPPING = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 4),
		/// Supports hardware stencil buffer
		RSC_HWSTENCIL = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 5),
		/// Supports hardware vertex and index buffers
		RSC_VBO = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 7),
		/// Supports vertex programs (vertex shaders)
		RSC_VERTEX_PROGRAM = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 9),
		/// Supports fragment programs (pixel shaders)
		RSC_FRAGMENT_PROGRAM = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 10),
		/// Supports performing a scissor test to exclude areas of the screen
		RSC_SCISSOR_TEST = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 11),
		/// Supports separate stencil updates for both front and back faces
		RSC_TWO_SIDED_STENCIL = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 12),
		/// Supports wrapping the stencil value at the range extremeties
		RSC_STENCIL_WRAP = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 13),
		/// Supports hardware occlusion queries
		RSC_HWOCCLUSION = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 14),
		/// Supports user clipping planes
		RSC_USER_CLIP_PLANES = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 15),
		/// Supports the VET_UBYTE4 vertex element type
		RSC_VERTEX_FORMAT_UBYTE4 = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 16),
		/// Supports infinite far plane projection
		RSC_INFINITE_FAR_PLANE = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 17),
		/// Supports hardware render-to-texture (bigger than framebuffer)
		RSC_HWRENDER_TO_TEXTURE = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 18),
		/// Supports float textures and render targets
		RSC_TEXTURE_FLOAT = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 19),
		/// Supports non-power of two textures
		RSC_NON_POWER_OF_2_TEXTURES = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 20),
		/// Supports 3d (volume) textures
		RSC_TEXTURE_3D = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 21),
		/// Supports basic point sprite rendering
		RSC_POINT_SPRITES = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 22),
		/// Supports extra point parameters (minsize, maxsize, attenuation)
		RSC_POINT_EXTENDED_PARAMETERS = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 23),
		/// Supports vertex texture fetch
		RSC_VERTEX_TEXTURE_FETCH = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 24),
		/// Supports mipmap LOD biasing
		RSC_MIPMAP_LOD_BIAS = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 25),
		/// Supports hardware geometry programs
		RSC_GEOMETRY_PROGRAM = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 26),
		/// Supports rendering to vertex buffers
		RSC_HWRENDER_TO_VERTEX_BUFFER = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON, 27),

		/// Supports compressed textures
		RSC_TEXTURE_COMPRESSION = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 0),
		/// Supports compressed textures in the DXT/ST3C formats
		RSC_TEXTURE_COMPRESSION_DXT = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 1),
		/// Supports compressed textures in the VTC format
		RSC_TEXTURE_COMPRESSION_VTC = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 2),
		/// Supports compressed textures in the PVRTC format
		RSC_TEXTURE_COMPRESSION_PVRTC = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 3),
		/// Supports fixed-function pipeline
		RSC_FIXED_FUNCTION = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 4),
		/// Supports MRTs with different bit depths
		RSC_MRT_DIFFERENT_BIT_DEPTHS = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 5),
		/// Supports Alpha to Coverage (A2C)
		RSC_ALPHA_TO_COVERAGE = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 6),
		/// Supports Blending operations other than +
		RSC_ADVANCED_BLEND_OPERATIONS = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 7),
		/// Supports a separate depth buffer for RTTs. D3D 9 & 10, OGL w/FBO (RSC_FBO implies this flag)
		RSC_RTT_SEPARATE_DEPTHBUFFER = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 8),
		/// Supports using the MAIN depth buffer for RTTs. D3D 9&10, OGL w/FBO support unknown
		/// (undefined behavior?), OGL w/ copy supports it
		RSC_RTT_MAIN_DEPTHBUFFER_ATTACHABLE = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 9),
		/// Supports attaching a depth buffer to an RTT that has width & height less or equal than RTT's.
		/// Otherwise must be of _exact_ same resolution. D3D 9, OGL 3.0 (not 2.0, not D3D10)
		RSC_RTT_DEPTHBUFFER_RESOLUTION_LESSEQUAL = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 10),
		/// Supports using vertex buffers for instance data
		RSC_VERTEX_BUFFER_INSTANCE_DATA = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 11),
		/// Supports using vertex buffers for instance data
		RSC_CAN_GET_COMPILED_SHADER_BUFFER = YUME_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 12),

		// ***** DirectX specific caps *****
		/// Is DirectX feature "per stage constants" supported
		RSC_PERSTAGECONSTANT = YUME_CAPS_VALUE(CAPS_CATEGORY_D3D9, 0),

		// ***** GL Specific Caps *****
		/// Supports openGL GLEW version 1.5
		RSC_GL1_5_NOVBO = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 1),
		/// Support for Frame Buffer Objects (FBOs)
		RSC_FBO = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 2),
		/// Support for Frame Buffer Objects ARB implementation (regular FBO is higher precedence)
		RSC_FBO_ARB = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 3),
		/// Support for Frame Buffer Objects ATI implementation (ARB FBO is higher precedence)
		RSC_FBO_ATI = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 4),
		/// Support for PBuffer
		RSC_PBUFFER = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 5),
		/// Support for GL 1.5 but without HW occlusion workaround
		RSC_GL1_5_NOHWOCCLUSION = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 6),
		/// Support for point parameters ARB implementation
		RSC_POINT_EXTENDED_PARAMETERS_ARB = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 7),
		/// Support for point parameters EXT implementation
		RSC_POINT_EXTENDED_PARAMETERS_EXT = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 8),
		/// Support for Separate Shader Objects
		RSC_SEPARATE_SHADER_OBJECTS = YUME_CAPS_VALUE(CAPS_CATEGORY_GL, 9)
	};

	struct YumeAPIExport DriverVersion
	{
		int major;
		int minor;
		int release;
		int build;

		DriverVersion()
		{
			major = minor = release = build = 0;
		}

		YumeString toString() const
		{
			StringUtil::StrStreamType str;
			str << major << "." << minor << "." << release << "." << build;
			return str.str();
		}

		void fromString(const YumeString& versionString)
		{
			StringVector tokens = StringUtil::split(versionString, ".");
			if (!tokens.empty())
			{
				major = YumeStringConverter::parseInt(tokens[0]);
				if (tokens.size() > 1)
					minor = YumeStringConverter::parseInt(tokens[1]);
				if (tokens.size() > 2)
					release = YumeStringConverter::parseInt(tokens[2]);
				if (tokens.size() > 3)
					build = YumeStringConverter::parseInt(tokens[3]);
			}

		}
	};

	enum GPUVendor
	{
		GPU_UNKNOWN = 0,
		GPU_NVIDIA = 1,
		GPU_ATI = 2,
		GPU_INTEL = 3,
		GPU_S3 = 4,
		GPU_MATROX = 5,
		GPU_3DLABS = 6,
		GPU_SIS = 7,
		GPU_IMAGINATION_TECHNOLOGIES = 8,
		GPU_APPLE = 9,
		GPU_NOKIA = 10,
		GPU_MS_SOFTWARE = 11,
		GPU_MS_WARP = 12,

		/// placeholder
		GPU_VENDOR_COUNT = 13
	};

	class YumeAPIExport YumeRendererCapabilities : public RenderObjAlloc
	{
	public:
		typedef YumeSet<YumeString>::type ShaderProfiles;
	private:
		/// This is used to build a database of RSC's
		/// if a RSC with same name, but newer version is introduced, the older one 
		/// will be removed
		DriverVersion mDriverVersion;
		/// GPU Vendor
		GPUVendor mVendor;

		static StringVector msGPUVendorStrings;
		static void initVendorStrings();

		/// The number of world matrices available
		yushort mNumWorldMatrices;
		/// The number of texture units available
		yushort mNumTextureUnits;
		/// The stencil buffer bit depth
		yushort mStencilBufferBitDepth;
		/// The number of matrices available for hardware blending
		yushort mNumVertexBlendMatrices;
		/// Stores the capabilities flags.
		int mCapabilities[CAPS_CATEGORY_COUNT];
		/// Which categories are relevant
		bool mCategoryRelevant[CAPS_CATEGORY_COUNT];
		/// The name of the device as reported by the render system
		YumeString mDeviceName;
		/// The identifier associated with the render system for which these capabilities are valid
		YumeString mRenderSystemName;

		/// The number of floating-point constants vertex programs support
		yushort mVertexProgramConstantFloatCount;
		/// The number of integer constants vertex programs support
		yushort mVertexProgramConstantIntCount;
		/// The number of boolean constants vertex programs support
		yushort mVertexProgramConstantBoolCount;
		/// The number of floating-point constants geometry programs support
		yushort mGeometryProgramConstantFloatCount;
		/// The number of integer constants vertex geometry support
		yushort mGeometryProgramConstantIntCount;
		/// The number of boolean constants vertex geometry support
		yushort mGeometryProgramConstantBoolCount;
		/// The number of floating-point constants fragment programs support
		yushort mFragmentProgramConstantFloatCount;
		/// The number of integer constants fragment programs support
		yushort mFragmentProgramConstantIntCount;
		/// The number of boolean constants fragment programs support
		yushort mFragmentProgramConstantBoolCount;
		/// The number of simultaneous render targets supported
		yushort mNumMultiRenderTargets;
		/// The maximum point size
		Real mMaxPointSize;
		/// Are non-POW2 textures feature-limited?
		bool mNonPOW2TexturesLimited;
		/// The number of vertex texture units supported
		yushort mNumVertexTextureUnits;
		/// Are vertex texture units shared with fragment processor?
		bool mVertexTextureUnitsShared;
		/// The number of vertices a geometry program can emit in a single run
		int mGeometryProgramNumOutputVertices;


		/// The list of supported shader profiles
		ShaderProfiles mSupportedShaderProfiles;

	public:
		YumeRendererCapabilities();
		virtual ~YumeRendererCapabilities();

		void Log();

		virtual size_t calculateSize() const { return 0; }

		/** Set the driver version. */
		void setDriverVersion(const DriverVersion& version)
		{
			mDriverVersion = version;
		}

		void parseDriverVersionFromString(const YumeString& versionString)
		{
			DriverVersion version;
			version.fromString(versionString);
			setDriverVersion(version);
		}


		DriverVersion getDriverVersion() const
		{
			return mDriverVersion;
		}

		GPUVendor getVendor() const
		{
			return mVendor;
		}

		void setVendor(GPUVendor v)
		{
			mVendor = v;
		}

		/// Parse and set vendor
		void parseVendorFromString(const YumeString& vendorString)
		{
			setVendor(vendorFromString(vendorString));
		}

		/// Convert a vendor string to an enum
		static GPUVendor vendorFromString(const YumeString& vendorString);
		/// Convert a vendor enum to a string
		static YumeString vendorToString(GPUVendor v);

		bool isDriverOlderThanVersion(DriverVersion v) const
		{
			if (mDriverVersion.major < v.major)
				return true;
			else if (mDriverVersion.major == v.major &&
				mDriverVersion.minor < v.minor)
				return true;
			else if (mDriverVersion.major == v.major &&
				mDriverVersion.minor == v.minor &&
				mDriverVersion.release < v.release)
				return true;
			else if (mDriverVersion.major == v.major &&
				mDriverVersion.minor == v.minor &&
				mDriverVersion.release == v.release &&
				mDriverVersion.build < v.build)
				return true;
			return false;
		}

		void setNumWorldMatrices(yushort num)
		{
			mNumWorldMatrices = num;
		}

		void setNumTextureUnits(yushort num)
		{
			mNumTextureUnits = num;
		}

		void setStencilBufferBitDepth(yushort num)
		{
			mStencilBufferBitDepth = num;
		}

		void setNumVertexBlendMatrices(yushort num)
		{
			mNumVertexBlendMatrices = num;
		}

		/// The number of simultaneous render targets supported
		void setNumMultiRenderTargets(yushort num)
		{
			mNumMultiRenderTargets = num;
		}

		yushort getNumWorldMatrices(void) const
		{
			return mNumWorldMatrices;
		}

		/** Returns the number of texture units the current output hardware
		supports.

		For use in rendering, this determines how many texture units the
		are available for multitexturing (i.e. rendering multiple
		textures in a single pass). Where a Material has multiple
		texture layers, it will try to use multitexturing where
		available, and where it is not available, will perform multipass
		rendering to achieve the same effect. This property only applies
		to the fixed-function pipeline, the number available to the
		programmable pipeline depends on the shader model in use.
		*/
		yushort getNumTextureUnits(void) const
		{
			return mNumTextureUnits;
		}

		/** Determines the bit depth of the hardware accelerated stencil
		buffer, if supported.
		@remarks
		If hardware stencilling is not supported, the software will
		provide an 8-bit software stencil.
		*/
		yushort getStencilBufferBitDepth(void) const
		{
			return mStencilBufferBitDepth;
		}

		/** Returns the number of matrices available to hardware vertex
		blending for this rendering system. */
		yushort getNumVertexBlendMatrices(void) const
		{
			return mNumVertexBlendMatrices;
		}

		/// The number of simultaneous render targets supported
		yushort getNumMultiRenderTargets(void) const
		{
			return mNumMultiRenderTargets;
		}

		/** Returns true if capability is render system specific
		*/
		bool isCapabilityRenderSystemSpecific(const Capabilities c) const
		{
			int cat = c >> YUME_CAPS_BITSHIFT;
			if (cat == CAPS_CATEGORY_GL || cat == CAPS_CATEGORY_D3D9)
				return true;
			return false;
		}

		/** Adds a capability flag
		*/
		void setCapability(const Capabilities c)
		{
			int index = (CAPS_CATEGORY_MASK & c) >> YUME_CAPS_BITSHIFT;
			// zero out the index from the stored capability
			mCapabilities[index] |= (c & ~CAPS_CATEGORY_MASK);
		}

		/** Remove a capability flag
		*/
		void unsetCapability(const Capabilities c)
		{
			int index = (CAPS_CATEGORY_MASK & c) >> YUME_CAPS_BITSHIFT;
			// zero out the index from the stored capability
			mCapabilities[index] &= (~c | CAPS_CATEGORY_MASK);
		}

		/** Checks for a capability
		*/
		bool hasCapability(const Capabilities c) const
		{
			int index = (CAPS_CATEGORY_MASK & c) >> YUME_CAPS_BITSHIFT;
			// test against
			if (mCapabilities[index] & (c & ~CAPS_CATEGORY_MASK))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		/** Adds the profile to the list of supported profiles
		*/
		void addShaderProfile(const YumeString& profile)
		{
			mSupportedShaderProfiles.insert(profile);

		}

		/** Remove a given shader profile, if present.
		*/
		void removeShaderProfile(const YumeString& profile)
		{
			mSupportedShaderProfiles.erase(profile);
		}

		/** Returns true if profile is in the list of supported profiles
		*/
		bool isShaderProfileSupported(const YumeString& profile) const
		{
			return (mSupportedShaderProfiles.end() != mSupportedShaderProfiles.find(profile));
		}


		/** Returns a set of all supported shader profiles
		* */
		const ShaderProfiles& getSupportedShaderProfiles() const
		{
			return mSupportedShaderProfiles;
		}


		/// The number of floating-point constants vertex programs support
		yushort getVertexProgramConstantFloatCount(void) const
		{
			return mVertexProgramConstantFloatCount;
		}
		/// The number of integer constants vertex programs support
		yushort getVertexProgramConstantIntCount(void) const
		{
			return mVertexProgramConstantIntCount;
		}
		/// The number of boolean constants vertex programs support
		yushort getVertexProgramConstantBoolCount(void) const
		{
			return mVertexProgramConstantBoolCount;
		}
		/// The number of floating-point constants geometry programs support
		yushort getGeometryProgramConstantFloatCount(void) const
		{
			return mGeometryProgramConstantFloatCount;
		}
		/// The number of integer constants geometry programs support
		yushort getGeometryProgramConstantIntCount(void) const
		{
			return mGeometryProgramConstantIntCount;
		}
		/// The number of boolean constants geometry programs support
		yushort getGeometryProgramConstantBoolCount(void) const
		{
			return mGeometryProgramConstantBoolCount;
		}
		/// The number of floating-point constants fragment programs support
		yushort getFragmentProgramConstantFloatCount(void) const
		{
			return mFragmentProgramConstantFloatCount;
		}
		/// The number of integer constants fragment programs support
		yushort getFragmentProgramConstantIntCount(void) const
		{
			return mFragmentProgramConstantIntCount;
		}
		/// The number of boolean constants fragment programs support
		yushort getFragmentProgramConstantBoolCount(void) const
		{
			return mFragmentProgramConstantBoolCount;
		}

		/// sets the device name for Render system
		void setDeviceName(const YumeString& name)
		{
			mDeviceName = name;
		}

		/// gets the device name for render system
		YumeString getDeviceName() const
		{
			return mDeviceName;
		}

		/// The number of floating-point constants vertex programs support
		void setVertexProgramConstantFloatCount(yushort c)
		{
			mVertexProgramConstantFloatCount = c;
		}
		/// The number of integer constants vertex programs support
		void setVertexProgramConstantIntCount(yushort c)
		{
			mVertexProgramConstantIntCount = c;
		}
		/// The number of boolean constants vertex programs support
		void setVertexProgramConstantBoolCount(yushort c)
		{
			mVertexProgramConstantBoolCount = c;
		}
		/// The number of floating-point constants geometry programs support
		void setGeometryProgramConstantFloatCount(yushort c)
		{
			mGeometryProgramConstantFloatCount = c;
		}
		/// The number of integer constants geometry programs support
		void setGeometryProgramConstantIntCount(yushort c)
		{
			mGeometryProgramConstantIntCount = c;
		}
		/// The number of boolean constants geometry programs support
		void setGeometryProgramConstantBoolCount(yushort c)
		{
			mGeometryProgramConstantBoolCount = c;
		}
		/// The number of floating-point constants fragment programs support
		void setFragmentProgramConstantFloatCount(yushort c)
		{
			mFragmentProgramConstantFloatCount = c;
		}
		/// The number of integer constants fragment programs support
		void setFragmentProgramConstantIntCount(yushort c)
		{
			mFragmentProgramConstantIntCount = c;
		}
		/// The number of boolean constants fragment programs support
		void setFragmentProgramConstantBoolCount(yushort c)
		{
			mFragmentProgramConstantBoolCount = c;
		}
		/// Maximum point screen size in pixels
		void setMaxPointSize(Real s)
		{
			mMaxPointSize = s;
		}
		/// Maximum point screen size in pixels
		Real getMaxPointSize(void) const
		{
			return mMaxPointSize;
		}
		/// Non-POW2 textures limited
		void setNonPOW2TexturesLimited(bool l)
		{
			mNonPOW2TexturesLimited = l;
		}
		/** Are non-power of two textures limited in features?
		@remarks
		If the RSC_NON_POWER_OF_2_TEXTURES capability is set, but this
		method returns true, you can use non power of 2 textures only if:
		<ul><li>You load them explicitly with no mip maps</li>
		<li>You don't use DXT texture compression</li>
		<li>You use clamp texture addressing</li></ul>
		*/
		bool getNonPOW2TexturesLimited(void) const
		{
			return mNonPOW2TexturesLimited;
		}

		/// Set the number of vertex texture units supported
		void setNumVertexTextureUnits(yushort n)
		{
			mNumVertexTextureUnits = n;
		}
		/// Get the number of vertex texture units supported
		yushort getNumVertexTextureUnits(void) const
		{
			return mNumVertexTextureUnits;
		}
		/// Set whether the vertex texture units are shared with the fragment processor
		void setVertexTextureUnitsShared(bool shared)
		{
			mVertexTextureUnitsShared = shared;
		}
		/// Get whether the vertex texture units are shared with the fragment processor
		bool getVertexTextureUnitsShared(void) const
		{
			return mVertexTextureUnitsShared;
		}

		/// Set the number of vertices a single geometry program run can emit
		void setGeometryProgramNumOutputVertices(int numOutputVertices)
		{
			mGeometryProgramNumOutputVertices = numOutputVertices;
		}
		/// Get the number of vertices a single geometry program run can emit
		int getGeometryProgramNumOutputVertices(void) const
		{
			return mGeometryProgramNumOutputVertices;
		}

		/// Get the identifier of the rendersystem from which these capabilities were generated
		YumeString getRenderSystemName(void) const
		{
			return mRenderSystemName;
		}
		///  Set the identifier of the rendersystem from which these capabilities were generated
		void setRenderSystemName(const YumeString& rs)
		{
			mRenderSystemName = rs;
		}

		/// Mark a category as 'relevant' or not, ie will it be reported
		void setCategoryRelevant(CapabilitiesCategory cat, bool relevant)
		{
			mCategoryRelevant[cat] = relevant;
		}

		/// Return whether a category is 'relevant' or not, ie will it be reported
		bool isCategoryRelevant(CapabilitiesCategory cat)
		{
			return mCategoryRelevant[cat];
		}

	};

}


//---------------------------------------------------------------------------------
#endif
//~End of __YumeRendererCapabilities_h__