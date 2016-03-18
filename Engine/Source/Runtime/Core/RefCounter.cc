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
#include "RefCounter.h"



namespace YumeEngine
{

	RefCounted::RefCounted():
		refCount_(new RefCount())
	{
		// Hold a weak ref to self to avoid possible double delete of the refcount
		(refCount_->weakRefs_)++;
	}

	RefCounted::~RefCounted()
	{
		assert(refCount_);
		assert(refCount_->refs_ == 0);
		assert(refCount_->weakRefs_ > 0);

		// Mark object as expired, release the self weak ref and delete the refcount if no other weak refs exist
		refCount_->refs_ = -1;
		(refCount_->weakRefs_)--;
		if(!refCount_->weakRefs_)
			delete refCount_;

		refCount_ = 0;
	}

	void RefCounted::AddRef()
	{
		assert(refCount_->refs_ >= 0);
		(refCount_->refs_)++;
	}

	void RefCounted::ReleaseRef()
	{
		assert(refCount_->refs_ > 0);
		(refCount_->refs_)--;
		if(!refCount_->refs_)
			delete this;
	}

	int RefCounted::Refs() const
	{
		return refCount_->refs_;
	}

	int RefCounted::WeakRefs() const
	{
		// Subtract one to not return the internally held reference
		return refCount_->weakRefs_ - 1;
	}

}
