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

#include "YumeRHI.h"
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
	YumeHash YumeShader::type_ = ("Shader");
	YumeShader::YumeShader()
		: timeStamp_(0),
		numVariations_(0)
	{
	}

	YumeShader::~YumeShader()
	{
	}

	bool YumeShader::ProcessSource(YumeString& code,YumeFile& file)
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;
		YumeIO* io_ = gYume->pIO;

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

			if(line.StartsWith("#include"))
			{
				String includeFileName = GetPath(file.GetName()) + line.substr(9).Replaced("\"","").Trimmed();

				SharedPtr<YumeFile> file_ = rm_->GetFile(includeFileName);

				if(!file_)
					return false;

				if(!ProcessSource(code,*file_))
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
		Vector<String> definesVec = defines.ToUpper().Split(' ');
		Sort(definesVec.begin(),definesVec.end());
		return String::Joined(definesVec," ");
	}
}
