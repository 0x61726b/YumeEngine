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
#ifndef __YumeDefaults_h__
#define __YumeDefaults_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeColor.h"
#include "Math/YumeMatrix4.h"
#include "Math/YumeMatrix3x4.h"
#include "Core/YumeVariant.h"

#include <boost/filesystem.hpp>

//----------------------------------------------------------------------------
namespace YumeEngine
{
	
	void CreateConfigFile(const boost::filesystem::path& path);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(const YumeString&,bool skipFirst = false);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(const char* cmdLine);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(const std::wstring& cmdLine);
	YumeAPIExport const YumeVector<YumeString>::type& ParseArguments(int argc,char** argv);

	YumeAPIExport unsigned GetStringListIndex(const YumeString& value,const YumeString* strings,unsigned defaultIndex,bool caseSensitive = false);
	YumeAPIExport unsigned GetStringListIndex(const char* value,const YumeString* strings,unsigned defaultIndex,bool caseSensitive = false);
	YumeAPIExport unsigned GetStringListIndex(const char* value,const char** strings,unsigned defaultIndex,bool caseSensitive = false);


	YumeAPIExport bool ToBool(const YumeString& source);
	YumeAPIExport bool ToBool(const char* source);
	YumeAPIExport float ToFloat(const YumeString& source);
	YumeAPIExport float ToFloat(const char* source);
	YumeAPIExport double ToDouble(const YumeString& source);
	YumeAPIExport double ToDouble(const char* source);
	YumeAPIExport int ToInt(const YumeString& source);
	YumeAPIExport int ToInt(const char* source);
	YumeAPIExport unsigned ToUInt(const YumeString& source);
	YumeAPIExport unsigned ToUInt(const char* source);
	YumeAPIExport YumeColor ToColor(const YumeString& source);
	YumeAPIExport YumeColor ToColor(const char* source);
	YumeAPIExport IntRect ToIntRect(const YumeString& source);
	YumeAPIExport IntRect ToIntRect(const char* source);
	YumeAPIExport IntVector2 ToIntVector2(const YumeString& source);
	YumeAPIExport IntVector2 ToIntVector2(const char* source);
	YumeAPIExport Quaternion ToQuaternion(const YumeString& source);
	YumeAPIExport Quaternion ToQuaternion(const char* source);
	YumeAPIExport Rect ToRect(const YumeString& source);
	YumeAPIExport Rect ToRect(const char* source);
	YumeAPIExport Vector2 ToVector2(const YumeString& source);
	YumeAPIExport Vector2 ToVector2(const char* source);
	YumeAPIExport Vector3 ToVector3(const YumeString& source);
	YumeAPIExport Vector3 ToVector3(const char* source);
	YumeAPIExport Vector4 ToVector4(const YumeString& source,bool allowMissingCoords = false);
	YumeAPIExport Vector4 ToVector4(const char* source,bool allowMissingCoords = false);
	YumeAPIExport Variant ToVectorVariant(const YumeString& source);
	YumeAPIExport Variant ToVectorVariant(const char* source);
	YumeAPIExport Matrix3 ToMatrix3(const YumeString& source);
	YumeAPIExport Matrix3 ToMatrix3(const char* source);
	YumeAPIExport Matrix3x4 ToMatrix3x4(const YumeString& source);
	YumeAPIExport Matrix3x4 ToMatrix3x4(const char* source);
	YumeAPIExport Matrix4 ToMatrix4(const YumeString& source);
	YumeAPIExport Matrix4 ToMatrix4(const char* source);
	YumeAPIExport YumeString ToString(void* value);
	YumeAPIExport YumeString ToStringHex(unsigned value);
	YumeAPIExport void StringToBuffer(YumeVector<unsigned char>::type& dest,const YumeString& source);
	YumeAPIExport void StringToBuffer(YumeVector<unsigned char>::type& dest,const char* source);
	YumeAPIExport void BufferToString(YumeString& dest, const void* data, unsigned size);

	YumeAPIExport bool IsDigit(unsigned ch);
	YumeAPIExport bool IsAlpha(unsigned ch);

	YumeAPIExport const YumeString& WCharToUtf8(const std::wstring&);

	YumeAPIExport YumeHash WtfIsThis(const YumeString& key);

	YumeAPIExport void ErrorDialog(const YumeString& title,const YumeString& message);

	YumeAPIExport void ErrorExit(const YumeString& message,int exitCode);

	YumeAPIExport unsigned CStringLength(const char* str);

	const YumeVector<YumeString>::type& GetArguments();
}


//----------------------------------------------------------------------------
#endif
