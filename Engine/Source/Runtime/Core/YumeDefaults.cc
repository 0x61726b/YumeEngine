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

		for(unsigned i = 0; i < cmdLine.size(); ++i)
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
			cmdEnd = cmdLine.size();
			if(!skipFirst)
				arguments.push_back(cmdLine.substr(cmdStart,cmdEnd - cmdStart));
		}
		for(unsigned i = 0; i < arguments.size(); ++i)
		{
			boost::replace_all(arguments[i],"\"","");
			boost::replace_all(arguments[i],"--","");
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
			cmdLine.append(fmt.str());
		}

		return ParseArguments(cmdLine);
	}

	YumeHash GenerateHash(const YumeString& key)
	{
		boost::hash<YumeString> hasher;
		return hasher(key);
	}


	unsigned GetStringListIndex(const YumeString& value,const YumeString* strings,unsigned defaultIndex,bool caseSensitive)
	{
		return GetStringListIndex(value.c_str(),strings,defaultIndex,caseSensitive);
	}

	unsigned GetStringListIndex(const char* value,const YumeString* strings,unsigned defaultIndex,bool caseSensitive)
	{
		unsigned i = 0;

		while(!strings[i].empty())
		{
			if(boost::iequals(strings[i],value))
				return i;
			++i;
		}

		return defaultIndex;
	}

	unsigned GetStringListIndex(const char* value, const char** strings, unsigned defaultIndex, bool caseSensitive)
	{
		unsigned i = 0;

		while(strings[i])
		{
			if(!boost::iequals(value,strings[i]))
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


	const YumeVector<YumeString>::type& GetArguments()
	{
		return arguments;
	}

}
