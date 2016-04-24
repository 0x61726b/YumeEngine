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

#ifndef __YumeRequired_h__
#define __YumeRequired_h__
//---------------------------------------------------------------------------------
#include "YumePlatform.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
#define YUME_VERSION_MAJOR 0
#define YUME_VERSION_MINOR 1
#define YUME_VERSION_PATCH 0
#define YUME_VERSION_SUFFIX ""
#define YUME_VERSION_NAME "Chitanda"

#define YUME_VERSION_NUMBER    ((YUME_VERSION_MAJOR << 16) | (YUME_VERSION_MINOR << 8) | YUME_VERSION_PATCH)
}
//---------------------------------------------------------------------------------
#include "YumeStdHeaders.h"
#include "YumeMemoryAllocatorConfig.h"
#include "Core/SharedPtr.h"
#include "Container/Str.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	typedef String YumeString;

	static YumeString EmptyString = "";

	class YumeEngine3D;
	class YumeMiscRenderer;
	class YumeRHI;
	class YumeResourceManager;
	class YumeIO;
	class YumeTime;
	class YumeWorkQueue;
	class YumeEnvironment;
	class YumeDebugRenderer;
	class YumeInput;
	class YumeObjectFactory;

	class Vector2;
	class Vector3;
	class Vector4;

	class Matrix3x4;
	class Matrix3;
	class Matrix4;

	class YumeShaderVariation;
	class YumeUI;

	class Quaternion;
}
//---------------------------------------------------------------------------------
#include "Math/YumeHash.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	template <typename T>
	struct YumeVector
	{
		typedef typename Vector<T> type;
		typedef typename Vector<T>::Iterator iterator;
		typedef typename Vector<T>::ConstIterator const_iterator;
	};

	template <typename T>
	struct YumePodVector
	{
		typedef typename PODVector<T> type;
		typedef typename PODVector<T>::Iterator iterator;
		typedef typename PODVector<T>::ConstIterator const_iterator;
	};
	template <typename K,typename V >
	struct YumeMap
	{
		typedef typename HashMap<K,V> type;
		typedef typename HashMap<K,V>::Iterator iterator;
		typedef typename HashMap<K,V>::ConstIterator const_iterator;
	};

	template <typename T>
	struct YumeHashSet
	{
		typedef typename HashSet<T> type;
		typedef typename HashSet<T>::Iterator iterator;
		typedef typename HashSet<T>::ConstIterator const_iterator;
	};

	typedef YumeVector<YumeString> StringVector;

#define YUME_SAFE_RELEASE(p) if(p) { p->Release(); p = 0; }
}

#if YUME_PLATFORM != YUME_PLATFORM_APPLE
#define YUME_SSE
#endif

#include <boost/shared_array.hpp>

#include "Core/YumeBase.h"

//---------------------------------------------------------------------------------
#endif
//~End of YumeRequired.h