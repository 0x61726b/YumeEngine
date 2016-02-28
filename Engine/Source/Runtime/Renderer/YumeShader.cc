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
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeShader.h"

#include "YumeRenderer.h"
#include "YumeShaderVariation.h"
#include "Core/YumeFile.h"
#include "Core/YumeIO.h"

#include "Engine/YumeEngine.h"

#include "Core/YumeDefaults.h"

#include "Renderer/YumeResourceManager.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>


namespace YumeEngine
{
	YumeHash YumeShader::shaderHash_ = GenerateHash("Shader");
	YumeShader::YumeShader()
		: timeStamp_(0),
		numVariations_(0)
	{
		shaderHash_ = GenerateHash("Shader");
	}

	YumeShader::~YumeShader()
	{
	}

	bool YumeShader::ProcessSource(YumeString& code,YumeFile& file)
	{
		YumeResourceManager* rm_ = YumeEngine3D::Get()->GetResourceManager();
		SharedPtr<YumeIO> io_ = YumeEngine3D::Get()->GetIO();

		YumeString fullPath = rm_->GetFullPath(file.GetName());
		unsigned timestamp = io_->GetLastModifiedTime(fullPath);

		if(timestamp > timeStamp_)
			timeStamp_ = timestamp;

		if(file.GetName() != GetName())
			rm_->StoreResourceDependency(this,file.GetName());

		while(!file.Eof())
		{
			YumeString line = file.ReadLine();

			if(line.empty())
				continue;

			if(boost::starts_with(line,"#include"))
			{
				YumeString includeFileName = (GetPath(file.GetName()) + line.substr(9));
				boost::replace_all(includeFileName,"\"","");
				boost::trim(includeFileName);

				SharedPtr<YumeFile> file_ = rm_->GetFile(includeFileName);

				if(!file_)
					return false;

				if(!ProcessSource(code,*file_.get()))
					return false;
			}
			else
			{
				code += line;
				code += "\n";
			}
		}

		return true;
	}

	YumeString YumeShader::NormalizeDefines(const YumeString& defines)
	{
		YumeString copy = defines;
		boost::to_upper(copy);
		
		YumeVector<YumeString>::type defs;
		boost::split(defs,copy,boost::is_any_of(" "));
		
		std::sort(defs.begin(),defs.end());
		return boost::join(defs," ");;
	}

	YumeHash YumeShader::GetType()
	{
		return shaderHash_;
	}
}
