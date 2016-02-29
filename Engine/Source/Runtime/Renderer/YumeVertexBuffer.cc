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
#include "Renderer/YumeVertexBuffer.h"
#include "Renderer/YumeRHI.h"




namespace YumeEngine
{
	const unsigned YumeVertexBuffer::elementSize[] =
	{
		3 * sizeof(float), // Position
		3 * sizeof(float), // Normal
		4 * sizeof(unsigned char), // Color
		2 * sizeof(float), // Texcoord1
		2 * sizeof(float), // Texcoord2
		3 * sizeof(float), // Cubetexcoord1
		3 * sizeof(float), // Cubetexcoord2
		4 * sizeof(float), // Tangent
		4 * sizeof(float), // Blendweights
		4 * sizeof(unsigned char), // Blendindices
		4 * sizeof(float), // Instancematrix1
		4 * sizeof(float), // Instancematrix2
		4 * sizeof(float), // Instancematrix3
		sizeof(int) // Objectindex
	};

	YumeVertexBuffer::YumeVertexBuffer():
		vertexCount_(0),
		elementMask_(0),
		lockState_(LOCK_NONE),
		lockStart_(0),
		lockCount_(0),
		lockScratchData_(0),
		dynamic_(false),
		shadowed_(false)
	{
	}

	YumeVertexBuffer::~YumeVertexBuffer()
	{
		
	}

	unsigned YumeVertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned vertexSize = 0;

		for(unsigned i = 0; i < MAX_VERTEX_ELEMENTS; ++i)
		{
			if(elementMask & (1 << i))
				vertexSize += elementSize[i];
		}

		return vertexSize;
	}

	unsigned YumeVertexBuffer::GetElementOffset(unsigned elementMask,VertexElement element)
	{
		unsigned offset = 0;

		for(unsigned i = 0; i != element; ++i)
		{
			if(elementMask & (1 << i))
				offset += elementSize[i];
		}

		return offset;
	}
}
