#pragma once

namespace OverEngine
{
	template<typename T>
	class Allocator
	{
	public:
		using value_type      =       T;

		using pointer         =       T*;
		using const_pointer   = const T*;

		using reference       =       T&;
		using const_reference = const T&;

		using size_type       = size_t;
		using difference_type = ptrdiff_t;

		/// Default constructor
		Allocator() throw() {}

		/// Copy constructor
		Allocator(const Allocator& other) throw() {}

		/// Copy constructor with another type
		template<typename U>
		Allocator(const Allocator<U>&) throw() {}

		/// Destructor
		~Allocator() {}

		/// Copy
		Allocator<T>& operator=(const Allocator& other) { return *this; }

		/// Copy with another type
		template<typename U>
		Allocator& operator=(const Allocator<U>& other) { return *this; }

		/// Get address of a reference
		pointer address(reference x) const { return &x; }

		/// Get const address of a const reference
		const_pointer address(const_reference x) const { return &x; }

		/// Allocate n elements of type T
		pointer allocate(size_type n, const void* hint = 0)
		{
			size_type size = n * sizeof(value_type);
			s_Allocations += size;
			return (pointer)::malloc(size);
		}

		/// Free memory of pointer p
		void deallocate(void* p, size_type n)
		{
			size_type size = n * sizeof(T);
			s_Allocations -= size;
			::free(p);
		}

		/// Call the constructor of p
		void construct(pointer p, const T& val)
		{
			// Placement new
			new ((T*)p) T(val);
		}

		/// Call the constructor of p with many arguments. C++11
		template<typename U, typename... Args>
		void construct(U* p, Args&&... args)
		{
			// Placement new
			::new((void*)p) U(std::forward<Args>(args)...);
		}

		/// Call the destructor of p
		void destroy(pointer p)
		{
			p->~T();
		}

		/// Call the destructor of p of type U
		template<typename U>
		void destroy(U* p)
		{
			p->~U();
		}

		size_type max_size() const { return size_type(-1); }

		template<typename U>
		struct rebind
		{
			using other = Allocator<U>;
		};

		static size_type GetAllocationSize() { return s_Allocations; }
	private:
		static size_type s_Allocations;
	};

	template<typename T>
	typename Allocator<T>::size_type Allocator<T>::s_Allocations = 0;
}