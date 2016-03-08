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
#include "YumeXmlParser.h"
#include "YumeEnvironment.h"

#include <boost/lexical_cast.hpp>

namespace YumeEngine
{
	template <typename T>
	T CastToType(const YumeString& str)
	{
		return boost::lexical_cast<T>(str);
	}


	YumeString GetStringValueFromXPath(const XmlNode& str)
	{
		return str.text().get();
	}


	VariantMap Parsers::ParseConfig(const YumeString& content)
	{
		VariantMap vm;

		pugi::xml_document doc;
		doc.load(content.c_str());

		XmlNode root = doc.child("Yume");
		XmlNode graphics = root.child("Graphics");
		XmlNode renderer = graphics.child("Renderer");
		XmlNode fullscreen = graphics.child("Fullscreen");
		XmlNode width = graphics.child("WindowWidth");
		XmlNode height = graphics.child("WindowHeight");
		XmlNode borderlessWindow = graphics.child("BorderlessWindow");
		XmlNode vsync = graphics.child("Vsync");
		XmlNode tripleBuffer = graphics.child("TripleBuffer");
		XmlNode multisample = graphics.child("MultiSample");


		vm["Renderer"] = YumeString(renderer.text().get());
		vm["Fullscreen"] = CastToType<bool>(GetStringValueFromXPath(fullscreen));
		vm["WindowWidth"] = CastToType<int>(GetStringValueFromXPath(width));
		vm["WindowHeight"] = CastToType<int>(GetStringValueFromXPath(height));
		vm["Borderless"] = CastToType<bool>(GetStringValueFromXPath(borderlessWindow));
		vm["Vsync"] = CastToType<bool>(GetStringValueFromXPath(vsync));
		vm["TripleBuffer"] = CastToType<bool>(GetStringValueFromXPath(tripleBuffer));
		vm["MultiSample"] = CastToType<int>(GetStringValueFromXPath(multisample));

		 // ToDo(arkenthera): Check if values are valid

		return vm;
	}
}
