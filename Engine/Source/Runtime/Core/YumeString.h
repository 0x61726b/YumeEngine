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
#ifndef __YumeString_h__
#define __YumeString_h__
//---------------------------------------------------------------------------------
#include "Core/YumeRequired.h"

#if YUME_COMPILER == YUME_COMPILER_GNUC && YUME_COMPILER_VERSION >= 310 && !defined(STLPORT)

// For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
#   if YUME_COMPILER_VERSION >= 430
#       include <tr1/unordered_map> 
#   else
#       include <ext/hash_map>
namespace __gnu_cxx
{
	template <> struct hash< YumeEngine::_YumeStringBase>
	{
		size_t operator()(const YumeEngine::_YumeStringBase _stringBase) const
		{
			/* This is the PRO-STL way, but it seems to cause problems with VC7.1
			and in some other cases (although I can't recreate it)
			hash<const char*> H;
			return H(_stringBase.c_str());
			*/
			/** This is OGRE's custom way */
			register size_t ret = 0;
			for (YumeEngine::_YumeStringBase::const_iterator it = _stringBase.begin(); it != _stringBase.end(); ++it)
				ret = 5 * ret + *it;

			return ret;
		}
	};
}
#   endif

#endif


namespace YumeEngine
{
	class YumeAPIExport StringUtil
	{
	public:
		typedef YumeStringStream StrStreamType;

		/** Removes any whitespace characters, be it standard space or
		TABs and so on.
		@remarks
		The user may specify wether they want to trim only the
		beginning or the end of the String ( the default action is
		to trim both).
		*/
		static void trim(YumeString& str, bool left = true, bool right = true);

		/** Returns a StringVector that contains all the substrings delimited
		by the characters in the passed <code>delims</code> argument.
		@param
		delims A list of delimiter characters to split by
		@param
		maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
		parameters is > 0, the splitting process will stop after this many splits, left to right.
		@param
		preserveDelims Flag to determine if delimiters should be saved as substrings
		*/
		static YumeVector<YumeString>::type split(const YumeString& str, const YumeString& delims = "\t\n ", unsigned int maxSplits = 0, bool preserveDelims = false);

		/** Returns a StringVector that contains all the substrings delimited
		by the characters in the passed <code>delims</code> argument,
		or in the <code>doubleDelims</code> argument, which is used to include (normal)
		delimeters in the tokenised string. For example, "strings like this".
		@param
		delims A list of delimiter characters to split by
		@param
		delims A list of double delimeters characters to tokenise by
		@param
		maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
		parameters is > 0, the splitting process will stop after this many splits, left to right.
		*/
		static YumeVector<YumeString>::type tokenise(const YumeString& str, const YumeString& delims = "\t\n ", const YumeString& doubleDelims = "\"", unsigned int maxSplits = 0);

		/** Lower-cases all the characters in the string.
		*/
		static void toLowerCase(YumeString& str);

		/** Upper-cases all the characters in the string.
		*/
		static void toUpperCase(YumeString& str);


		/** Returns whether the string begins with the pattern passed in.
		@param pattern The pattern to compare with.
		@param lowerCase If true, the start of the string will be lower cased before
		comparison, pattern should also be in lower case.
		*/
		static bool startsWith(const YumeString& str, const YumeString& pattern, bool lowerCase = true);

		/** Returns whether the string ends with the pattern passed in.
		@param pattern The pattern to compare with.
		@param lowerCase If true, the end of the string will be lower cased before
		comparison, pattern should also be in lower case.
		*/
		static bool endsWith(const YumeString& str, const YumeString& pattern, bool lowerCase = true);

		/** Method for standardising paths - use forward slashes only, end with slash.
		*/
		static YumeString standardisePath(const YumeString &init);
		/** Returns a normalized version of a file path
		This method can be used to make file path strings which point to the same directory
		but have different texts to be normalized to the same text. The function:
		- Transforms all backward slashes to forward slashes.
		- Removes repeating slashes.
		- Removes initial slashes from the beginning of the path.
		- Removes ".\" and "..\" meta directories.
		- Sets all characters to lowercase (if requested)
		@param init The file path to normalize.
		@param makeLowerCase If true, transforms all characters in the string to lowercase.
		*/
		static YumeString normalizeFilePath(const YumeString& init, bool makeLowerCase = true);


		/** Method for splitting a fully qualified filename into the base name
		and path.
		@remarks
		Path is standardised as in standardisePath
		*/
		static void splitFilename(const YumeString& qualifiedName,
			YumeString& outBasename, YumeString& outPath);

		/** Method for splitting a fully qualified filename into the base name,
		extension and path.
		@remarks
		Path is standardised as in standardisePath
		*/
		static void splitFullFilename(const YumeString& qualifiedName,
			YumeString& outBasename, YumeString& outExtention,
			YumeString& outPath);

		/** Method for splitting a filename into the base name
		and extension.
		*/
		static void splitBaseFilename(const YumeString& fullName,
			YumeString& outBasename, YumeString& outExtention);


		/** Simple pattern-matching routine allowing a wildcard pattern.
		@param str String to test
		@param pattern Pattern to match against; can include simple '*' wildcards
		@param caseSensitive Whether the match is case sensitive or not
		*/
		static bool match(const YumeString& str, const YumeString& pattern, bool caseSensitive = true);


		/** replace all instances of a sub-string with a another sub-string.
		@param source Source string
		@param replaceWhat Sub-string to find and replace
		@param replaceWithWhat Sub-string to replace with (the new sub-string)
		@return An updated string with the sub-string replaced
		*/
		static const YumeString replaceAll(const YumeString& source, const YumeString& replaceWhat, const YumeString& replaceWithWhat);

		/// Constant blank string, useful for returning by ref where local does not exist
		static const YumeString BLANK;
	};



#if YUME_COMPILER == YUME_COMPILER_GNUC && YUME_COMPILER_VERSION >= 310 && !defined(STLPORT)
#   if YUME_COMPILER_VERSION < 430
	typedef ::__gnu_cxx::hash< _YumeStringBase > _StringHash;
#   else
	typedef ::std::tr1::hash< _YumeStringBase > _StringHash;
#   endif
#elif YUME_COMPILER == YUME_COMPILER_CLANG
#   if defined(_LIBCPP_VERSION)
	typedef ::std::hash< _YumeStringBase > _StringHash;
#   else
	typedef ::std::tr1::hash< _YumeStringBase > _StringHash;
#   endif
#elif YUME_COMPILER == YUME_COMPILER_MSVC && YUME_COMPILER_VERSION >= 1600 && !defined(STLPORT) // VC++ 10.0
	typedef ::std::tr1::hash< _YumeStringBase > _StringHash;
#elif !defined( _STLP_HASH_FUN_H )
	typedef stdext::hash_compare< _YumeStringBase, std::less< _YumeStringBase > > _StringHash;
#else
	typedef std::hash< _YumeStringBase > _StringHash;
#endif 
}

//---------------------------------------------------------------------------------
#endif
//~End of __YumeRendererCapabilities_h__#pragma once
