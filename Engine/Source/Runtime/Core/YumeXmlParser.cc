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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeXmlParser.h"
#include "YumeEnvironment.h"
namespace YumeEngine
{


	YumeMap<YumeString,YumeString>::type Parsers::ParseConfig(const YumeString& content)
	{
		ConfigMap ret;

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

		for(XmlNode graphicsChild = graphics.first_child(); graphicsChild; graphicsChild = graphicsChild.next_sibling())
		{
			/* TODO Check if values are valid */

			const char* name = graphicsChild.name();
			const char* val = graphicsChild.text().get();

			ret.insert(ConfigMap::value_type(name,val));
		}



		return ret;
	}
}
