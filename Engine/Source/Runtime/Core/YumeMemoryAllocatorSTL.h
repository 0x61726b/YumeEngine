




























//---------------------------------------------------------------------------------
#ifndef __YumeMemoryAllocatorSTL_h__
#define __YumeMemoryAllocatorSTL_h__
//---------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeHeaderPrefix.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	// Base STL allocator class.
	template<typename T>
	struct STLAllocatorBase
	{	// base class for generic allocators
		typedef T value_type;
	};

	// Base STL allocator class. (const T version).
	template<typename T>
	struct STLAllocatorBase<const T>
	{	// base class for generic allocators for const T
		typedef T value_type;
	};

	template
		<
		typename T,
		typename AllocPolicy
		>
	class YumeSTLAllocator : public STLAllocatorBase<T>
	{
	public:
		
		typedef STLAllocatorBase<T>			Base;
		typedef typename Base::value_type	value_type;
		typedef value_type*					pointer;
		typedef const value_type*			const_pointer;
		typedef value_type&					reference;
		typedef const value_type&			const_reference;
		typedef std::size_t					size_type;
		typedef std::ptrdiff_t				difference_type;


		
		template<typename U>
		struct rebind
		{
			typedef YumeSTLAllocator<U, AllocPolicy> other;
		};

		
		inline explicit YumeSTLAllocator()
		{ }

		
		virtual ~YumeSTLAllocator()
		{ }

		
		inline YumeSTLAllocator(YumeSTLAllocator const&)
		{ }

		
		template <typename U>
		inline YumeSTLAllocator(YumeSTLAllocator<U, AllocPolicy> const&)
		{ }

		
		template <typename U, typename P>
		inline YumeSTLAllocator(YumeSTLAllocator<U, P> const&)
		{ }

		
		inline pointer allocate(size_type count,
			typename std::allocator<void>::const_pointer ptr = 0)
		{
			(void)ptr;
			// convert request to bytes
			register size_type sz = count*sizeof(T);
			pointer p = static_cast<pointer>(AllocPolicy::allocateBytes(sz));
			return p;
		}

		
		inline void deallocate(pointer ptr, size_type)
		{
			// convert request to bytes, but we can't use this?
			// register size_type sz = count*sizeof( T );
			AllocPolicy::deallocateBytes(ptr);
		}

		pointer address(reference x) const
		{
			return &x;
		}

		const_pointer address(const_reference x) const
		{
			return &x;
		}

		size_type max_size() const throw()
		{
			// maximum size this can handle, delegate
			return AllocPolicy::getMaxAllocationSize();
		}

		void construct(pointer p)
		{
			// call placement new
			new(static_cast<void*>(p)) T();
		}

		void construct(pointer p, const T& val)
		{
			// call placement new
			new(static_cast<void*>(p)) T(val);
		}

		void destroy(pointer p)
		{
			// do we have to protect against non-classes here?
			// some articles suggest yes, some no
			p->~T();
		}

	};

	
	
	template<typename T, typename T2, typename P>
	inline bool operator==(YumeSTLAllocator<T, P> const&,
		YumeSTLAllocator<T2, P> const&)
	{
		// same alloc policy (P), memory can be freed
		return true;
	}

	
	
	template<typename T, typename P, typename OtherAllocator>
	inline bool operator==(YumeSTLAllocator<T, P> const&,
		OtherAllocator const&)
	{
		return false;
	}
	
	
	template<typename T, typename T2, typename P>
	inline bool operator!=(YumeSTLAllocator<T, P> const&,
		YumeSTLAllocator<T2, P> const&)
	{
		// same alloc policy (P), memory can be freed
		return false;
	}

	
	
	template<typename T, typename P, typename OtherAllocator>
	inline bool operator!=(YumeSTLAllocator<T, P> const&,
		OtherAllocator const&)
	{
		return true;
	}

}

//---------------------------------------------------------------------------------
#include "YumeHeaderSuffix.h"
//---------------------------------------------------------------------------------
#endif
