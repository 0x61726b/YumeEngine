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
#ifndef __YumeSortAlgorithms_h__
#define __YumeSortAlgorithms_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	static const int QUICKSORT_THRESHOLD = 16;

	// Based on Comparison of several sorting algorithms by Juha Nieminen
	// http://warp.povusers.org/SortComparison/

	/// Perform insertion sort on an array.
	template <class T> void InsertionSort(T begin,T end)
	{
		for(T i = begin + 1; i < end; ++i)
		{
			T::value_type temp = *i;
			T j = i;
			while(j > begin && temp < *(j - 1))
			{
				*j = *(j - 1);
				--j;
			}
			*j = temp;
		}
	}

	/// Perform insertion sort on an array using a compare function.
	template <class T,class U> void InsertionSort(T begin,T end,U compare)
	{
		for(T i = begin + 1; i < end; ++i)
		{
			T::value_type temp = *i;
			T j = i;
			while(j > begin && compare(temp,*(j - 1)))
			{
				*j = *(j - 1);
				--j;
			}
			*j = temp;
		}
	}

	/// Perform quick sort initial pass on an array. Does not sort fully.
	template <class T> void InitialQuickSort(T begin,T end)
	{
		while(end - begin > QUICKSORT_THRESHOLD)
		{
			// Choose the pivot by median
			T pivot = begin + ((end - begin) / 2);
			if(*begin < *pivot && *(end - 1) < *begin)
				pivot = begin;
			else if(*(end - 1) < *pivot && *begin < *(end - 1))
				pivot = end - 1;

			// Partition and sort recursively
			T i = begin - 1;
			T j = end;
			T::value_type pivotValue = *pivot;
			for(;;)
			{
				while(pivotValue < *(--j));
				while(*(++i) < pivotValue);
				if(i < j)
					std::swap(*i,*j);
				else
					break;
			}

			InitialQuickSort(begin,j + 1);
			begin = j + 1;
		}
	}

	/// Perform quick sort initial pass on an array using a compare function. Does not sort fully.
	template <class T,class U> void InitialQuickSort(T begin,T end,U compare)
	{
		while(end - begin > QUICKSORT_THRESHOLD)
		{
			// Choose the pivot by median
			T pivot = begin + ((end - begin) / 2);
			if(compare(*begin,*pivot) && compare(*(end - 1),*begin))
				pivot = begin;
			else if(compare(*(end - 1),*pivot) && compare(*begin,*(end - 1)))
				pivot = end - 1;

			// Partition and sort recursively
			T i = begin - 1;
			T j = end;
			T::value_type pivotValue = *pivot;
			for(;;)
			{
				while(compare(pivotValue,*(--j)));
				while(compare(*(++i),pivotValue));
				if(i < j)
					std::swap(*i,*j);
				else
					break;
			}

			InitialQuickSort(begin,j + 1,compare);
			begin = j + 1;
		}
	}

	/// Sort in ascending order using quicksort for initial passes, then an insertion sort to finalize.
	template <class T> void Sort(T begin,T end)
	{
		InitialQuickSort(begin,end);
		InsertionSort(begin,end);
	}

	/// Sort in ascending order using quicksort for initial passes, then an insertion sort to finalize, using a compare function.
	template <class T,class U> void Sort(T begin,T end,U compare)
	{
		InitialQuickSort(begin,end,compare);
		InsertionSort(begin,end,compare);
	}

}


//----------------------------------------------------------------------------
#endif
