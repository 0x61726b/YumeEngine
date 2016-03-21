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
#include "YumeOctreeQuery.h"
#include "Renderer/YumeDrawable.h"


namespace YumeEngine
{

	Intersection PointOctreeQuery::TestOctant(const BoundingBox& box,bool inside)
	{
		if(inside)
			return INSIDE;
		else
			return box.IsInside(point_);
	}

	void PointOctreeQuery::TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
	{
		while(start != end)
		{
			YumeDrawable* drawable = *start++;

			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
			{
				if(inside || drawable->GetWorldBoundingBox().IsInside(point_))
					result_.push_back(drawable);
			}
		}
	}

	Intersection SphereOctreeQuery::TestOctant(const BoundingBox& box,bool inside)
	{
		if(inside)
			return INSIDE;
		else
			return sphere_.IsInside(box);
	}

	void SphereOctreeQuery::TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
	{
		while(start != end)
		{
			YumeDrawable* drawable = *start++;

			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
			{
				if(inside || sphere_.IsInsideFast(drawable->GetWorldBoundingBox()))
					result_.push_back(drawable);
			}
		}
	}

	Intersection BoxOctreeQuery::TestOctant(const BoundingBox& box,bool inside)
	{
		if(inside)
			return INSIDE;
		else
			return box_.IsInside(box);
	}

	void BoxOctreeQuery::TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
	{
		while(start != end)
		{
			YumeDrawable* drawable = *start++;

			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
			{
				if(inside || box_.IsInsideFast(drawable->GetWorldBoundingBox()))
					result_.push_back(drawable);
			}
		}
	}

	Intersection FrustumOctreeQuery::TestOctant(const BoundingBox& box,bool inside)
	{
		if(inside)
			return INSIDE;
		else
			return frustum_.IsInside(box);
	}

	void FrustumOctreeQuery::TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
	{
		while(start != end)
		{
			YumeDrawable* drawable = *start++;

			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
			{
				if(inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
					result_.push_back(drawable);
			}
		}
	}


	Intersection AllContentOctreeQuery::TestOctant(const BoundingBox& box,bool inside)
	{
		return INSIDE;
	}

	void AllContentOctreeQuery::TestDrawables(YumeDrawable** start,YumeDrawable** end,bool inside)
	{
		while(start != end)
		{
			YumeDrawable* drawable = *start++;

			if((drawable->GetDrawableFlags() & drawableFlags_) && (drawable->GetViewMask() & viewMask_))
				result_.push_back(drawable);
		}
	}

}
