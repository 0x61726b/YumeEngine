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
#ifndef __YumeApplication_h__
#define __YumeApplication_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeMain.h"
#include "YumeDefaults.h"
#include "YumeVariant.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeEngine3D;

	class YumeAPIExport YumeApplication : public YumeBase
	{
	public:
		YumeApplication();
		~YumeApplication();

		virtual void Setup() { }

		virtual void Start() { }

		virtual void Stop() { }

		int Run();

	protected:
		SharedPtr<YumeEngine3D> engine_;
		VariantMap engineVariants_;
		int exitCode_;
	};

#define YUME_DEFINE_ENTRY_POINT(className) \
int RunApplication() \
	{ \
    YumeEngine::SharedPtr<className> application(new className); \
    return application->Run(); \
	} \
YUME_MAIN(RunApplication());

#define YUME_TEST_SUITE_ENTRY(className) \
int RunTestSuite() \
	{ \
    YumeEngine::SharedPtr<className> application(new className); \
    application->Run(); \
	application->Stop(); \
	return 0 \
	} \
YUME_MAIN(RunTestSuite());
}



//----------------------------------------------------------------------------
#endif
