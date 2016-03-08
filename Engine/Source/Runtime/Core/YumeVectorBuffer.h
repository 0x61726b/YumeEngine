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
#ifndef __YumeVectorBuffer_h__
#define __YumeVectorBuffer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeFile;

	
	class YumeAPIExport VectorBuffer
	{
	public:
		/// Construct an empty buffer.
		VectorBuffer();
		/// Construct from another buffer.
		VectorBuffer(const YumeVector<unsigned char>::type& data);
		/// Construct from a memory area.
		VectorBuffer(const void* data,unsigned size);
		/// Construct from a stream.
		VectorBuffer(YumeFile& source,unsigned size);

		/// Read bytes from the buffer. Return number of bytes actually read.
		virtual unsigned Read(void* dest,unsigned size);
		/// Set position from the beginning of the buffer.
		virtual unsigned Seek(unsigned position);
		/// Write bytes to the buffer. Return number of bytes actually written.
		virtual unsigned Write(const void* data,unsigned size);

		/// Set data from another buffer.
		void SetData(const YumeVector<unsigned char>::type& data);
		/// Set data from a memory area.
		void SetData(const void* data,unsigned size);
		/// Set data from a stream.
		void SetData(YumeFile& source,unsigned size);
		/// Reset to zero size.
		void Clear();
		/// Set size.
		void Resize(unsigned size);

		/// Return data.
		const unsigned char* GetData() const { return size_ ? &buffer_[0] : 0; }

		/// Return non-const data.
		unsigned char* GetModifiableData() { return size_ ? &buffer_[0] : 0; }

		/// Return the buffer.
		const YumeVector<unsigned char>::type& GetBuffer() const { return buffer_; }

		unsigned GetSize() const { return size_; }

	private:
		/// Dynamic data buffer.
		YumeVector<unsigned char>::type buffer_;

		unsigned size_;
		unsigned position_;
	};
}


//----------------------------------------------------------------------------
#endif
