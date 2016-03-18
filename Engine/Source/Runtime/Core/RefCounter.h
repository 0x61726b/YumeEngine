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
#ifndef __RefCounter_h__
#define __RefCounter_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Reference count structure.
	struct RefCount
	{
		/// Construct.
		RefCount():
			refs_(0),
			weakRefs_(0)
		{
		}

		/// Destruct.
		~RefCount()
		{
			// Set reference counts below zero to fire asserts if this object is still accessed
			refs_ = -1;
			weakRefs_ = -1;
		}

		/// Reference count. If below zero, the object has been destroyed.
		int refs_;
		/// Weak reference count.
		int weakRefs_;
	};

	/// Base class for intrusively reference-counted objects. These are noncopyable and non-assignable.
	class YumeAPIExport RefCounted
	{
	public:
		/// Construct. Allocate the reference count structure and set an initial self weak reference.
		RefCounted();
		/// Destruct. Mark as expired and also delete the reference count structure if no outside weak references exist.
		virtual ~RefCounted();

		/// Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
		void AddRef();
		/// Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
		void ReleaseRef();
		/// Return reference count.
		int Refs() const;
		/// Return weak reference count.
		int WeakRefs() const;

		/// Return pointer to the reference count structure.
		RefCount* RefCountPtr() { return refCount_; }

	private:
		/// Prevent copy construction.
		RefCounted(const RefCounted& rhs);
		/// Prevent assignment.
		RefCounted& operator =(const RefCounted& rhs);

		/// Pointer to the reference count structure.
		RefCount* refCount_;
	};
}


//----------------------------------------------------------------------------
#endif
