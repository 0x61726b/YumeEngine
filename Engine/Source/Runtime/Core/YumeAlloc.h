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
#ifndef __YumeAlloc_h__
#define __YumeAlloc_h__
//----------------------------------------------------------------------------
#include <EABase/eabase.h>
#include <new>
#include "YumeRequired.h"

    void* operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/);
    void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/);
    void* operator new[](size_t size, size_t alignment);
    void* operator new[](size_t size, size_t alignment, const std::nothrow_t&)EA_THROW_SPEC_NEW_NONE();
  

//----------------------------------------------------------------------------
#endif
