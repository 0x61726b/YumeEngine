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
#ifndef __YumeDecompresser_h__
#define __YumeDecompresser_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Renderer/YumeImage.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	
	YumeAPIExport void
		DecompressImageDXT(unsigned char* dest,const void* blocks,int width,int height,int depth,CompressedFormat format);
	
	YumeAPIExport void DecompressImageETC(unsigned char* dest,const void* blocks,int width,int height);
	
	YumeAPIExport void DecompressImagePVRTC(unsigned char* dest,const void* blocks,int width,int height,CompressedFormat format);
	
	YumeAPIExport void FlipBlockVertical(unsigned char* dest,unsigned char* src,CompressedFormat format);
	
	YumeAPIExport void FlipBlockHorizontal(unsigned char* dest,unsigned char* src,CompressedFormat format);
}


//----------------------------------------------------------------------------
#endif
