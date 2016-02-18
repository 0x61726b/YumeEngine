//----------------------------------------------------------------------------
//Chiika Api
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
// File : YumeRandom.h
// Date : 2.18.2016
// Comments : 
//
//----------------------------------------------------------------------------

#include "YumeRequired.h"

namespace YumeEngine
{
	/// Set the random seed. The default seed is 1.
	YumeAPIExport void SetRandomSeed(unsigned seed);
	/// Return the current random seed.
	YumeAPIExport unsigned GetRandomSeed();
	/// Return a random number between 0-32767. Should operate similarly to MSVC rand().
	YumeAPIExport int Rand();
	/// Return a standard normal distributed number.
	YumeAPIExport float RandStandardNormal();
}