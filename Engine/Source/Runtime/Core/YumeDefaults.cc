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
#include "YumeHeaders.h"
#include "YumeDefaults.h"
#include "YumeFile.h"

#include "YumeXmlParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/format.hpp>
#include <boost/functional/hash.hpp>

#include <cstdio>
#include <fcntl.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <SDL.h>

namespace YumeEngine
{
	static YumeVector<YumeString>::type arguments;


	unsigned CStringLength(const char* str)
	{
		if(!str)
			return 0;
#ifdef _MSC_VER
		return (unsigned)strlen(str);
#else
		const char* ptr = str;
		while(*ptr)
			++ptr;
		return (unsigned)(ptr - str);
#endif
	}

	unsigned CountElements(const char* buffer,char separator)
	{
		if(!buffer)
			return 0;

		const char* endPos = buffer + CStringLength(buffer);
		const char* pos = buffer;
		unsigned ret = 0;

		while(pos < endPos)
		{
			if(*pos != separator)
				break;
			++pos;
		}

		while(pos < endPos)
		{
			const char* start = pos;

			while(start < endPos)
			{
				if(*start == separator)
					break;

				++start;
			}

			if(start == endPos)
			{
				++ret;
				break;
			}

			const char* end = start;

			while(end < endPos)
			{
				if(*end != separator)
					break;

				++end;
			}

			++ret;
			pos = end;
		}

		return ret;
	}

	void CreateConfigFile(const boost::filesystem::path& path)
	{
		pugi::xml_document doc;

		XmlNode root = doc.append_child("Yume");
		XmlNode graphics = root.append_child("Graphics");
		XmlNode renderer = graphics.append_child("Renderer");
		XmlNode fullscreen = graphics.append_child("Fullscreen");
		XmlNode width = graphics.append_child("WindowWidth");
		XmlNode height = graphics.append_child("WindowHeight");
		XmlNode borderlessWindow = graphics.append_child("BorderlessWindow");
		XmlNode vsync = graphics.append_child("Vsync");
		XmlNode tripleBuffer = graphics.append_child("TripleBuffer");
		XmlNode multisample = graphics.append_child("MultiSample");

		//Set default values
		renderer.text().set("Direct3D11");
		fullscreen.text().set(0);
		width.text().set(1280);
		height.text().set(720);
		borderlessWindow.text().set(0);
		vsync.text().set(0);
		tripleBuffer.text().set(0);
		multisample.text().set(1);
		fullscreen.text().set(0);


		doc.save_file(path.generic_wstring().c_str());
	}

	const YumeVector<YumeString>::type& ParseArguments(const YumeString& cmdLine,bool skipFirst)
	{
		arguments.clear();

		unsigned cmdStart = 0,cmdEnd = 0;
		bool inCmd = false;
		bool inQuote = false;

		for(unsigned i = 0; i < cmdLine.length(); ++i)
		{
			if(cmdLine[i] == '\"')
				inQuote = !inQuote;
			if(cmdLine[i] == ' ' && !inQuote)
			{
				if(inCmd)
				{
					inCmd = false;
					cmdEnd = i;
					// Do not store the first argument (executable name)
					if(!skipFirst)
						arguments.push_back(cmdLine.substr(cmdStart,cmdEnd - cmdStart));
					skipFirst = false;
				}
			}
			else
			{
				if(!inCmd)
				{
					inCmd = true;
					cmdStart = i;
				}
			}
		}
		if(inCmd)
		{
			cmdEnd = cmdLine.length();
			if(!skipFirst)
				arguments.push_back(cmdLine.substr(cmdStart,cmdEnd - cmdStart));
		}
		for(unsigned i = 0; i < arguments.size(); ++i)
		{
			arguments[i].Replace("\"","",false);
			arguments[i].Replace("--","");
		}

		return arguments;
	}

	const YumeVector<YumeString>::type& ParseArguments(const char* cmdLine)
	{
		return ParseArguments(YumeString(cmdLine));
	}

	const YumeVector<YumeString>::type& ParseArguments(int argc,char** argv)
	{
		YumeString cmdLine;

		for(int i = 0; i < argc; ++i)
		{
			boost::format fmt = boost::format("\"%s\" ") % (const char*)argv[i];
			cmdLine.append(fmt.str().c_str());
		}

		return ParseArguments(cmdLine);
	}

	unsigned GetStringListIndex(const String& value,const String* strings,unsigned defaultIndex,bool caseSensitive)
	{
		return GetStringListIndex(value.c_str(),strings,defaultIndex,caseSensitive);
	}

	unsigned GetStringListIndex(const char* value,const String* strings,unsigned defaultIndex,bool caseSensitive)
	{
		unsigned i = 0;

		while(!strings[i].empty())
		{
			if(!strings[i].Compare(value,caseSensitive))
				return i;
			++i;
		}

		return defaultIndex;
	}

	unsigned GetStringListIndex(const char* value,const char** strings,unsigned defaultIndex,bool caseSensitive)
	{
		unsigned i = 0;

		while(strings[i])
		{
			if(!String::Compare(value,strings[i],caseSensitive))
				return i;
			++i;
		}

		return defaultIndex;
	}



	void ErrorDialog(const YumeString& title,const YumeString& message)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),message.c_str(),0);
	}

	void ErrorExit(const YumeString& message,int exitCode)
	{
		if(!message.empty())
			ErrorDialog("Error!",message);

		exit(exitCode);
	}


	bool ToBool(const YumeString& source)
	{
		return ToBool(source.c_str());
	}

	bool ToBool(const char* source)
	{
		unsigned length = CStringLength(source);

		for(unsigned i = 0; i < length; ++i)
		{
			char c = (char)tolower(source[i]);
			if(c == 't' || c == 'y' || c == '1')
				return true;
			else if(c != ' ' && c != '\t')
				break;
		}

		return false;
	}


	int ToInt(const YumeString& source)
	{
		return ToInt(source.c_str());
	}

	int ToInt(const char* source)
	{
		if(!source)
			return 0;

		// Explicitly ask for base 10 to prevent source starts with '0' or '0x' from being converted to base 8 or base 16, respectively
		return (int)strtol(source,0,10);
	}

	float ToFloat(const YumeString& source)
	{
		return ToFloat(source.c_str());
	}

	float ToFloat(const char* source)
	{
		if(!source)
			return 0;

		return (float)strtod(source,0);
	}

	double ToDouble(const YumeString& source)
	{
		return ToDouble(source.c_str());
	}

	double ToDouble(const char* source)
	{
		if(!source)
			return 0;

		return strtod(source,0);
	}

	bool IsDigit(unsigned ch)
	{
		return ch < 256 ? isdigit(ch) != 0 : false;
	}

	bool IsAlpha(unsigned ch)
	{
		return ch < 256 ? isalpha(ch) != 0 : false;
	}

	unsigned ToUInt(const YumeString& source)
	{
		return ToUInt(source.c_str());
	}

	unsigned ToUInt(const char* source)
	{
		if(!source)
			return 0;

		return (unsigned)strtoul(source,0,10);
	}

	YumeColor ToColor(const YumeString& source)
	{
		return ToColor(source.c_str());
	}

	YumeColor ToColor(const char* source)
	{
		YumeColor ret;

		unsigned elements = CountElements(source,' ');
		if(elements < 3)
			return ret;

		char* ptr = (char*)source;
		ret.r_ = (float)strtod(ptr,&ptr);
		ret.g_ = (float)strtod(ptr,&ptr);
		ret.b_ = (float)strtod(ptr,&ptr);
		if(elements > 3)
			ret.a_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	IntRect ToIntRect(const YumeString& source)
	{
		return ToIntRect(source.c_str());
	}

	IntRect ToIntRect(const char* source)
	{
		IntRect ret(IntRect::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 4)
			return ret;

		char* ptr = (char*)source;
		ret.left_ = (int)strtol(ptr,&ptr,10);
		ret.top_ = (int)strtol(ptr,&ptr,10);
		ret.right_ = (int)strtol(ptr,&ptr,10);
		ret.bottom_ = (int)strtol(ptr,&ptr,10);

		return ret;
	}

	IntVector2 ToIntVector2(const YumeString& source)
	{
		return ToIntVector2(source.c_str());
	}

	IntVector2 ToIntVector2(const char* source)
	{
		IntVector2 ret(IntVector2::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 2)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (int)strtol(ptr,&ptr,10);
		ret.y_ = (int)strtol(ptr,&ptr,10);

		return ret;
	}

	Rect ToRect(const YumeString& source)
	{
		return ToRect(source.c_str());
	}

	Rect ToRect(const char* source)
	{
		Rect ret(Rect::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 4)
			return ret;

		char* ptr = (char*)source;
		ret.min_.x_ = (float)strtod(ptr,&ptr);
		ret.min_.y_ = (float)strtod(ptr,&ptr);
		ret.max_.x_ = (float)strtod(ptr,&ptr);
		ret.max_.y_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	Quaternion ToQuaternion(const YumeString& source)
	{
		return ToQuaternion(source.c_str());
	}

	Quaternion ToQuaternion(const char* source)
	{
		unsigned elements = CountElements(source,' ');
		char* ptr = (char*)source;

		if(elements < 3)
			return Quaternion::IDENTITY;
		else if(elements < 4)
		{
			// 3 coords specified: conversion from Euler angles
			float x,y,z;
			x = (float)strtod(ptr,&ptr);
			y = (float)strtod(ptr,&ptr);
			z = (float)strtod(ptr,&ptr);

			return Quaternion(x,y,z);
		}
		else
		{
			// 4 coords specified: full quaternion
			Quaternion ret;
			ret.w_ = (float)strtod(ptr,&ptr);
			ret.x_ = (float)strtod(ptr,&ptr);
			ret.y_ = (float)strtod(ptr,&ptr);
			ret.z_ = (float)strtod(ptr,&ptr);

			return ret;
		}
	}

	Vector2 ToVector2(const YumeString& source)
	{
		return ToVector2(source.c_str());
	}

	Vector2 ToVector2(const char* source)
	{
		Vector2 ret(Vector2::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 2)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (float)strtod(ptr,&ptr);
		ret.y_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	Vector3 ToVector3(const YumeString& source)
	{
		return ToVector3(source.c_str());
	}

	Vector3 ToVector3(const char* source)
	{
		Vector3 ret(Vector3::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 3)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (float)strtod(ptr,&ptr);
		ret.y_ = (float)strtod(ptr,&ptr);
		ret.z_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	Vector4 ToVector4(const YumeString& source,bool allowMissingCoords)
	{
		return ToVector4(source.c_str(),allowMissingCoords);
	}

	Vector4 ToVector4(const char* source,bool allowMissingCoords)
	{
		Vector4 ret(Vector4::ZERO);

		unsigned elements = CountElements(source,' ');
		char* ptr = (char*)source;

		if(!allowMissingCoords)
		{
			if(elements < 4)
				return ret;

			ret.x_ = (float)strtod(ptr,&ptr);
			ret.y_ = (float)strtod(ptr,&ptr);
			ret.z_ = (float)strtod(ptr,&ptr);
			ret.w_ = (float)strtod(ptr,&ptr);

			return ret;
		}
		else
		{
			if(elements > 0)
				ret.x_ = (float)strtod(ptr,&ptr);
			if(elements > 1)
				ret.y_ = (float)strtod(ptr,&ptr);
			if(elements > 2)
				ret.z_ = (float)strtod(ptr,&ptr);
			if(elements > 3)
				ret.w_ = (float)strtod(ptr,&ptr);

			return ret;
		}
	}

	Variant ToVectorVariant(const YumeString& source)
	{
		return ToVectorVariant(source.c_str());
	}

	Variant ToVectorVariant(const char* source)
	{
		Variant ret;
		unsigned elements = CountElements(source,' ');

		switch(elements)
		{
		case 1:
			ret.FromString(VAR_FLOAT,source);
			break;

		case 2:
			ret.FromString(VAR_VECTOR2,source);
			break;

		case 3:
			ret.FromString(VAR_VECTOR3,source);
			break;

		case 4:
			ret.FromString(VAR_VECTOR4,source);
			break;

		case 9:
			ret.FromString(VAR_MATRIX3,source);
			break;

		case 12:
			ret.FromString(VAR_MATRIX3X4,source);
			break;

		case 16:
			ret.FromString(VAR_MATRIX4,source);
			break;

		default:
			assert(false);  // Should not get here
			break;
		}

		return ret;
	}

	Matrix3 ToMatrix3(const YumeString& source)
	{
		return ToMatrix3(source.c_str());
	}

	Matrix3 ToMatrix3(const char* source)
	{
		Matrix3 ret(Matrix3::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 9)
			return ret;

		char* ptr = (char*)source;
		ret.m00_ = (float)strtod(ptr,&ptr);
		ret.m01_ = (float)strtod(ptr,&ptr);
		ret.m02_ = (float)strtod(ptr,&ptr);
		ret.m10_ = (float)strtod(ptr,&ptr);
		ret.m11_ = (float)strtod(ptr,&ptr);
		ret.m12_ = (float)strtod(ptr,&ptr);
		ret.m20_ = (float)strtod(ptr,&ptr);
		ret.m21_ = (float)strtod(ptr,&ptr);
		ret.m22_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	Matrix3x4 ToMatrix3x4(const YumeString& source)
	{
		return ToMatrix3x4(source.c_str());
	}

	Matrix3x4 ToMatrix3x4(const char* source)
	{
		Matrix3x4 ret(Matrix3x4::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 12)
			return ret;

		char* ptr = (char*)source;
		ret.m00_ = (float)strtod(ptr,&ptr);
		ret.m01_ = (float)strtod(ptr,&ptr);
		ret.m02_ = (float)strtod(ptr,&ptr);
		ret.m03_ = (float)strtod(ptr,&ptr);
		ret.m10_ = (float)strtod(ptr,&ptr);
		ret.m11_ = (float)strtod(ptr,&ptr);
		ret.m12_ = (float)strtod(ptr,&ptr);
		ret.m13_ = (float)strtod(ptr,&ptr);
		ret.m20_ = (float)strtod(ptr,&ptr);
		ret.m21_ = (float)strtod(ptr,&ptr);
		ret.m22_ = (float)strtod(ptr,&ptr);
		ret.m23_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	Matrix4 ToMatrix4(const YumeString& source)
	{
		return ToMatrix4(source.c_str());
	}

	Matrix4 ToMatrix4(const char* source)
	{
		Matrix4 ret(Matrix4::ZERO);

		unsigned elements = CountElements(source,' ');
		if(elements < 16)
			return ret;

		char* ptr = (char*)source;
		ret.m00_ = (float)strtod(ptr,&ptr);
		ret.m01_ = (float)strtod(ptr,&ptr);
		ret.m02_ = (float)strtod(ptr,&ptr);
		ret.m03_ = (float)strtod(ptr,&ptr);
		ret.m10_ = (float)strtod(ptr,&ptr);
		ret.m11_ = (float)strtod(ptr,&ptr);
		ret.m12_ = (float)strtod(ptr,&ptr);
		ret.m13_ = (float)strtod(ptr,&ptr);
		ret.m20_ = (float)strtod(ptr,&ptr);
		ret.m21_ = (float)strtod(ptr,&ptr);
		ret.m22_ = (float)strtod(ptr,&ptr);
		ret.m23_ = (float)strtod(ptr,&ptr);
		ret.m30_ = (float)strtod(ptr,&ptr);
		ret.m31_ = (float)strtod(ptr,&ptr);
		ret.m32_ = (float)strtod(ptr,&ptr);
		ret.m33_ = (float)strtod(ptr,&ptr);

		return ret;
	}

	YumeString ToString(void* value)
	{
		return ToStringHex((unsigned)(size_t)value);
	}

	YumeString ToStringHex(unsigned value)
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer,"%08x",value);
		return YumeString(tempBuffer);
	}


	void StringToBuffer(YumeVector<unsigned char>::type& dest,const YumeString& source)
	{
		StringToBuffer(dest,source.c_str());
	}

	void StringToBuffer(YumeVector<unsigned char>::type& dest,const char* source)
	{
		if(!source)
		{
			dest.clear();
			return;
		}

		unsigned size = CountElements(source,' ');
		dest.resize(size);

		bool inSpace = true;
		unsigned index = 0;
		unsigned value = 0;

		// Parse values
		const char* ptr = source;
		while(*ptr)
		{
			if(inSpace && *ptr != ' ')
			{
				inSpace = false;
				value = (unsigned)(*ptr - '0');
			}
			else if(!inSpace && *ptr != ' ')
			{
				value *= 10;
				value += *ptr - '0';
			}
			else if(!inSpace && *ptr == ' ')
			{
				dest[index++] = (unsigned char)value;
				inSpace = true;
			}

			++ptr;
		}

		// Write the final value
		if(!inSpace && index < size)
			dest[index] = (unsigned char)value;
	}


	void BufferToString(YumeString& dest,const void* data,unsigned size)
	{
		// Precalculate needed string size
		const unsigned char* bytes = (const unsigned char*)data;
		unsigned length = 0;
		for(unsigned i = 0; i < size; ++i)
		{
			// Room for separator
			if(i)
				++length;

			// Room for the value
			if(bytes[i] < 10)
				++length;
			else if(bytes[i] < 100)
				length += 2;
			else
				length += 3;
		}

		dest.resize(length);
		unsigned index = 0;

		// Convert values
		for(unsigned i = 0; i < size; ++i)
		{
			if(i)
				dest[index++] = ' ';

			if(bytes[i] < 10)
			{
				dest[index++] = '0' + bytes[i];
			}
			else if(bytes[i] < 100)
			{
				dest[index++] = (char)('0' + bytes[i] / 10);
				dest[index++] = (char)('0' + bytes[i] % 10);
			}
			else
			{
				dest[index++] = (char)('0' + bytes[i] / 100);
				dest[index++] = (char)('0' + bytes[i] % 100 / 10);
				dest[index++] = (char)('0' + bytes[i] % 10);
			}
		}
	}

	const YumeVector<YumeString>::type& GetArguments()
	{
		return arguments;
	}

}
