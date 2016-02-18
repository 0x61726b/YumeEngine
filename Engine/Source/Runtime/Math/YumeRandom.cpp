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

#include "YumeHeaders.h"
#include "YumeRandom.h"

namespace YumeEngine
{
	static unsigned randomSeed = 1;

	void SetRandomSeed(unsigned seed)
	{
		randomSeed = seed;
	}

	unsigned GetRandomSeed()
	{
		return randomSeed;
	}

	int Rand()
	{
		randomSeed = randomSeed * 214013 + 2531011;
		return (randomSeed >> 16) & 32767;
	}

	float RandStandardNormal()
	{
		float val = 0.0f;
		for(int i = 0; i < 12; i++)
			val += Rand() / 32768.0f;
		val -= 6.0f;

		// Now val is approximatly standard normal distributed
		return val;
	}
}