//--------------------------------------------------------------------------------------------------
//
//	QUEUE
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2015 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
//
//
//--------------------------------------------------------------------------------------------------
//
/// @file   circular_queue.h
/// @brief  An STL-style fixed-size circular queue
//
//--------------------------------------------------------------------------------------------------

#pragma once
#ifndef circular_queue_h_
#define circular_queue_h_

//------------------------
//	INCLUDES
//------------------------
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <cstdint>
#include <type_traits>

//	----------------------------------------------------------------------------
//	CLASS		circular_queue
//  ----------------------------------------------------------------------------
///	@brief		Fixed capacity, STL-style, templated circular buffer.
///	@details	see http://en.wikipedia.org/wiki/Circular_buffer for details of
///				how circular buffers work.\n\n
///
///				This implementation shares many features of a double-ended queue.
///				This structure allows for the individual elements to be accessed directly through
///				random access iterators, with storage handled automatically by over-writing elements
///				from the opposite end of the container as it grows. As long as elements are added
///				consistently to EITHER the front or the back, once full, the container will expand
///				by over-writing the oldest element. If elements are added to both sides of the
///				buffer, the behavior is effectively user defined.\n\n
///
///				Therefore, the circular_queue provides a functionality similar to vectors,
///				but with efficient insertion and deletion of elements also at the beginning of the
///				sequence, and not only at its end. Also, like vectors (and unlike dequeues), all
///				elements are stored in contiguous memory locations, and offset access IS allowed.\n\n
///
///				Both vectors and the circular_queue provide a very similar interface and
///				can be used for similar purposes, but internally both work in quite different ways :
///				While vectors use a single array that needs to be occasionally reallocated for growth,
///				the elements of a circular_queue are over-written once the buffer reaches
///				its maximum capacity, ensuring elements are inserted and accessed in constant time and
///				with a uniform sequential interface (through iterators), as opposed to the vector's
///				amortized constant time. This allows them to grow more efficiently under certain
///				circumstances, especially with very long sequences, where reallocation's become more
///				expensive, and real-time applications where stale data is not as useful as current data.\n\n
///
///				For operations that involve frequent insertion or removals of elements at positions
///				other than the beginning or the end, circular_queues perform worse and
///				have less consistent iterators and references than lists and forward lists.\n\n
///
///				This class can be moved very quickly (constant time). It may be slow to copy.\n
///
///				<b>Container properties</b>\n
///				<i>Sequence</i>\n
///				Elements in sequence containers are ordered in a strict linear sequence. Individual
///				elements are accessed by their position in this sequence.\n
///				<i>Fixed-capacity array</i>\n
///				Allows direct access to any element in the sequence and provides fast addition /
///				removal of elements at the beginning or the end of the sequence.\n
///				<i>Allocator - aware</i>\n
///				The container uses an allocator object to dynamically handle its storage needs.\n
//  ----------------------------------------------------------------------------

template<class T, class Alloc = std::allocator<T>>
class circular_queue
{
public:
	//////////////////////////////////////////////////////////////////////////
	//		FORWARD DECLARATIONS
	//////////////////////////////////////////////////////////////////////////
	class iterator;
	class const_iterator;

	//////////////////////////////////////////////////////////////////////////
	//		TYPEDEFS
	//////////////////////////////////////////////////////////////////////////

	using allocator_type         = Alloc;                                    ///< Type of object used to dynamically allocate memory.
	using difference_type        = ptrdiff_t;                                ///< A signed integral type, identical to:iterator_traits<iterator>::difference_type
	using size_type              = size_t;                                   ///< An unsigned integral type that can represent any non-negative value of difference_type.
	using value_type             = T;                                        ///< The template parameter (T), representing the values stored in the container.
	using pointer                = T*;                                       ///< For the default allocator: value_type*.
	using const_pointer          = const T*;                                 ///< Type for pointer to constant T object.
	using reference              = T&;                                       ///< Type for data references.
	using const_reference        = const T&;                                 ///< Type for reference to constant T object.
	using reverse_iterator       = std::reverse_iterator<iterator>;          ///< Type for reverse iterators.
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;    ///< Type for constant reverse iterators.

	/// pointer with parity
	struct queue_pointer
	{
		queue_pointer(pointer p, bool par)
		    : ptr(p)
		    , parity(par){};
		pointer ptr;
		bool    parity;
	};

	/// constant pointer with parity
	struct const_queue_pointer
	{
		const_queue_pointer(pointer p, bool par)
		    : ptr(p)
		    , parity(par){};
		const_pointer ptr;
		bool          parity;
	};

	//	----------------------------------------------------------------------------
	//	CLASS		const_iterator
	//  ----------------------------------------------------------------------------
	///	@brief		constant iterator for the circular_queue class.
	///	@details
	//  ----------------------------------------------------------------------------
	class const_iterator
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//		TYPEDEFS
		//////////////////////////////////////////////////////////////////////////

		using iterator_category = std::random_access_iterator_tag;
		using value_type        = const T;
		using difference_type   = ptrdiff_t;
		using pointer           = const T*;
		using reference         = const T&;

		//////////////////////////////////////////////////////////////////////////
		//		FRIENDS
		//////////////////////////////////////////////////////////////////////////

		friend class circular_queue;

		//////////////////////////////////////////////////////////////////////////
		//		MEMBERS
		//////////////////////////////////////////////////////////////////////////

		/**
         * @brief		Constructor
         * @details		Creates an initialized iterator.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> N/A.\n
         *				<b> Data Races:</b> N/A.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	buffer pointer to the underlying container.
         * @param[in]	start pointer to the iterators starting offset
         */
		const_iterator(const circular_queue* buffer, pointer start, bool parity) noexcept
		    : m_buffer(buffer)
		    , m_pointer(start, parity){

		      };

		const_iterator(const circular_queue* buffer, queue_pointer pointer) noexcept
		    : m_buffer(buffer)
		    , m_pointer(pointer){

		      };

		const_iterator()
		    : m_buffer(nullptr)
		    , m_pointer(nullptr, false){

		      };

		const_iterator(const const_iterator& other) = default;

		const_iterator& operator=(const const_iterator& other) = default;

		const_iterator(const_iterator&& other) noexcept
		    : m_buffer(std::move(other.m_buffer))
		    , m_pointer(std::move(other.m_pointer))
		{
		}

		const_iterator& operator=(const_iterator&& other) noexcept
		{
			m_buffer  = std::move(other.m_buffer);
			m_pointer = std::move(other.m_pointer);
			return *this;
		}

		/**
         * @brief		destructor.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         */
		inline ~const_iterator() noexcept = default;

		/**
        * @brief		Dereference operator
        * @details		Returns a reference to the element pointed to by the iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> Undefined if the iterator is not valid.\n
        * @returns		A reference to the element pointed by the iterator.
        */
		inline reference operator*() const
		{
			assert(m_buffer);
			return *(this->m_pointer.ptr);
		}

		/**
        * @brief		Dereference operator
        * @details		Returns a pointer to the element pointed to by the iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Unchanged.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> Undefined if the iterator is not valid.\n
        * @returns		A pointer to the element pointed by the iterator.
        */
		inline pointer operator&() const
		{
			assert(m_buffer);
			return this->m_pointer.ptr;
		}

		/**
        * @brief		Dereference operator
        * @details		Returns a pointer to the element pointed to by the iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Unchanged.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> Undefined if the iterator is not valid.\n
        * @returns		A pointer to the element pointed by the iterator.
        */
		inline pointer operator->() const
		{
			assert(m_buffer);
			return this->m_pointer.ptr;
		}

		/**
        * @brief		Dereference iterator with offset
        * @details		Accesses the element located n positions away from the element currently pointed
        *				to by the iterator. If such an element does not exist, it causes <i>undefined
        *				behavior</i>.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Unchanged.\n
        *				<b> Data Races:</b> The object is accessed. Depending on the return type, the
        *				value returned may be used to access or modify elements.\n
        *				<b> Exception Safety:</b> Undefined behavior if <i>n</i> is out of range.\n
        * @param[in]	n Number of elements to offset. Member type difference_type is an alias of the
        *				base container's own difference type.
        * @returns		The element n positions away from the element currently pointed by the iterator.
        */
		inline reference operator[](difference_type n) const
		{
			assert(m_buffer);
			const_iterator tmp = *this + n;
			return *tmp;
		}

		/**
         * @brief		equality operator
         * @details		Two iterators are equal if they point to the same object and have the same
         *				parity value (i.e. one isn't wrapped around the buffer from the other).\n
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> No changes.\n
         *				<b> Data Races:</b> The container is accessed.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	rhs right-hand-side of the equation.
         * @returns		true if both iterators point to the same object.
         */
		inline bool operator==(const const_iterator& rhs) const noexcept
		{
			// member-wise ==
			assert(this->m_buffer == rhs.m_buffer);
			return ((this->m_pointer.ptr == rhs.m_pointer.ptr) && (this->m_pointer.parity == rhs.m_pointer.parity));
		}

		/**
         * @brief		inequality operator
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> No changes.\n
         *				<b> Data Races:</b> The container is accessed.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	rhs right-hand-side of the equation.
         * @returns		true if both iterators point to different objects.
         */
		inline bool operator!=(const const_iterator& rhs) const
		{
			return !(operator==(rhs));
		}

		/**
         * @brief		Increment iterator position (prefix)
         * @details		Advances the iterator by 1 position.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Valid IFF the iterator is incrementable.\n
         *				<b> Data Races:</b> The object is modified.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @returns		A reference to the incremented iterator.
         */
		inline const_iterator& operator++() noexcept
		{
			return operator+=(1);
		}

		/**
         * @brief		Increment iterator position (postfix)
         * @details		Advances the iterator by 1 position.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Valid IFF the iterator is incrementable.\n
         *				<b> Data Races:</b> The object is modified.\n
         *				<b> Exception Safety:</b> Strong guarantee:  if the function throws an
         *					exception, there are no side effects.\n
         * @returns		A copy of the iterator before it was incremented.
         */
		inline const_iterator operator++(int)
		{
            const_iterator tmp(*this);
			     operator++();
			return tmp;
		}

		/**
         * @brief		Advance iterator.
         * @details		Advances the iterator by n element positions.\n
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Results in undefined behavior if the element at
         *					position <i>n</i> does not exist.\n
         *				<b> Data Races:</b> The object is modified.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[]	n
         * @returns		circular_queueIterator&
         */
		inline const_iterator& operator+=(difference_type n) noexcept
		{
			assert(m_buffer && m_pointer.ptr);
			m_pointer = m_buffer->increment(m_pointer, n);

			return *this;
		}

		/**
         * @brief		Addition operator.
         * @details		Returns an iterator pointing to the element located n positions away from the
         *				element the iterator currently points to.\n
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Undefined behavior if the element <i>n</i> positions
         *				away is out of bounds.\n
         *				<b> Data Races:</b> The object is accessed but NOT modified.\n
         *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
         *					exception, there are no side effects.\n
         * @param[in]	n number of elements to offset.
         * @returns		An iterator pointing to the element n positions away.
         */
		inline const_iterator operator+(difference_type n) const noexcept
		{
			assert(m_buffer && m_pointer.ptr);
			queue_pointer p(m_pointer);

			p = m_buffer->increment(p, n);

			return const_iterator(m_buffer, p);
		}

		/**
         * @brief		Decrease iterator position (prefix)
         * @details		Decreases the iterator by one position.\n
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
         *					decrementable, otherwise no changes.\n
         *				<b> Data Races:</b> The object is modified.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @returns		A reference to an iterator pointing to the post-decrement iterator.
         */
		inline const_iterator& operator--() noexcept
		{
			return operator-=(1);
		}

		/**
         * @brief		Decrease iterator position (postfix)
         * @details		Decreases the iterator by one position.\n
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
         *					decrementable, otherwise no changes.\n
         *				<b> Data Races:</b> The object is accessed but NOT modified.\n
         *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
         *					exception, there are no side effects.\n
         * @returns		A iterator pointing to the pre-decremented element.
         */
		inline const_iterator operator--(int)
		{
            const_iterator tmp(*this);
			     operator--();
			return tmp;
		}

		/**
         * @brief		Retrocede iterator
         * @details		Decreases the iterator by n element positions.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
         *					decrementable, otherwise no changes.\n
         *				<b> Data Races:</b> The object is modified.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	n Number of elements to offset. Member type difference_type is an alias of the
         *				base container's own difference type.\n
         * @returns		the iterator itself, decremented by n positions.
         */
		inline const_iterator& operator-=(difference_type n) noexcept
		{
			assert(m_buffer && m_pointer.ptr);
			m_pointer = m_buffer->decrement(m_pointer, n);

			return *this;
		}

		/**
         * @brief		subtraction operator
         * @details		Returns an iterator whose position is n elements before the current position
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
         *					decrementable, otherwise no changes.\n
         *				<b> Data Races:</b> the object is accessed but NOT modified.\n
         *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
         *					exception, there are no side effects.\n
         * @param[in]	n Number of elements to offset. Member type difference_type is an alias of the
         *				base container's own difference type.\n
         * @returns		An iterator decremented n positions from the current iterator position.
         */
		inline const_iterator operator-(difference_type n) const
		{
			assert(m_buffer && m_pointer.ptr);
			queue_pointer p(m_pointer);

			p = m_buffer->decrement(p, n);

			return const_iterator(m_buffer, p);
		}

		/**
         * @brief		subtraction operator
         * @details		Returns the distance between two iterators.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
         *					decrementable, otherwise no changes.\n
         *				<b> Data Races:</b> the object is accessed but NOT modified.\n
         *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
         *					exception, there are no side effects.\n
         * @param[in]	other iterator to determine distance from.\n
         * @returns		number of elements between the two iterators.
         */
		inline difference_type operator-(const const_iterator& other) const
		{
			assert(m_buffer);
			assert(m_buffer == other.m_buffer);
			assert(m_pointer.ptr && other.m_pointer.ptr);

			if (m_pointer.parity == other.m_pointer.parity)
			{
				return (m_pointer.ptr - other.m_pointer.ptr);
			}
			else
			{
				return (m_buffer->m_capacity - (other.m_pointer.ptr - m_pointer.ptr));
			}
		}

		/**
         * @brief		less-than operator
         * @details		Performs the appropriate comparison.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> No changes.\n
         *				<b> Data Races:</b> The object is accessed.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	rhs iterator for the right-hand side of the comparison.
         * @returns		true if this iterator is less than <i>rhs</i>.
         */
		inline bool operator<(const const_iterator& rhs) const noexcept
		{
			// check that the iterators are compatible
			assert(this->m_buffer == rhs.m_buffer);

			// it's possible the buffer is wrapped around, such that:
			//            T                     H
			// begin|--2--|---------------------|---------1---|end
			// this can be tested for by checking the parity bit. If the parity's match, they are not
			// wrapped. If the parity's don't match, they are wrapped.
			if (this->m_pointer.parity == rhs.m_pointer.parity)
			{
				return (this->m_pointer.ptr < rhs.m_pointer.ptr);
			}
			else
			{
				return this->m_pointer.ptr > rhs.m_pointer.ptr;
			}
		}

		/**
         * @brief		less-than-or-equal operator
         * @details		Performs the appropriate comparison.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> No changes.\n
         *				<b> Data Races:</b> The object is accessed.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	rhs iterator for the right-hand side of the comparison.
         * @returns		true if this iterator is less than or equal to <i>rhs</i>.
         */
		inline bool operator<=(const const_iterator& rhs) const
		{
			// define in terms of less than
			return (this->m_pointer.ptr == rhs.m_pointer.ptr || operator<(rhs));
		}

		/**
         * @brief		greater-than operator
         * @details		Performs the appropriate comparison.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> No changes.\n
         *				<b> Data Races:</b> The object is accessed.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	rhs iterator for the right-hand side of the comparison.
         * @returns		true if this iterator is greater than <i>rhs</i>.
         */
		inline bool operator>(const const_iterator& rhs) const
		{
			// define in terms of less than
			return !(operator<=(rhs));
		}

		/**
         * @brief		greater-than-or-equal operator
         * @details		Performs the appropriate comparison.
         *				<b> Complexity:</b> Constant.\n
         *				<b> Iterator Validity:</b> No changes.\n
         *				<b> Data Races:</b> The object is accessed.\n
         *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
         *					exceptions.\n
         * @param[in]	rhs iterator for the right-hand side of the comparison.
         * @returns		true if this iterator is greater than or equal to <i>rhs</i>.
         */
		inline bool operator>=(const const_iterator& rhs) const
		{
			// define in terms of less than
			return !(operator<(rhs));
		}

	protected:
		const circular_queue* m_buffer = nullptr;    ///< pointer to the buffer object that this iterates on.
		queue_pointer         m_pointer;             ///< Iterator position.
	};

	//	----------------------------------------------------------------------------
	//	CLASS		iterator
	//  ----------------------------------------------------------------------------
	///	@brief		iterator for the circular_queue class.
	///	@details
	//  ----------------------------------------------------------------------------
	class iterator
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//		TYPEDEFS
		//////////////////////////////////////////////////////////////////////////

		using iterator_category = std::random_access_iterator_tag;
		using value_type        = T;
		using difference_type   = ptrdiff_t;
		using pointer           = T*;
		using reference         = T&;

		//////////////////////////////////////////////////////////////////////////
		//		FRIENDS
		//////////////////////////////////////////////////////////////////////////

		friend class circular_queue;

		//////////////////////////////////////////////////////////////////////////
		//		MEMBERS
		//////////////////////////////////////////////////////////////////////////

		/**
        * @brief		Constructor
        * @details		Creates an initialized iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> N/A.\n
        *				<b> Data Races:</b> N/A.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	buffer pointer to the underlying container.
        * @param[in]	start pointer to the iterator's starting offset
        */
		iterator(const circular_queue* const buffer, pointer start, bool parity) noexcept
		    : m_buffer(buffer)
		    , m_pointer(start, parity){

		      };

		iterator(const circular_queue* const buffer, queue_pointer pointer) noexcept
		    : m_buffer(buffer)
		    , m_pointer(pointer){

		      };

		iterator()
		    : m_buffer(nullptr)
		    , m_pointer(nullptr, false){

		      };

		iterator(const iterator& other) = default;
		iterator& operator=(const iterator& other) = default;

		iterator(iterator&& other) noexcept
		    : m_buffer(std::move(other.m_buffer))
		    , m_pointer(std::move(other.m_pointer))
		{
		}

		iterator& operator=(iterator&& other) noexcept
		{
			m_buffer  = std::move(other.m_buffer);
			m_pointer = std::move(other.m_pointer);
			return *this;
		}

		/**
        * @brief		destructor.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        */
		inline ~iterator() noexcept = default;

		/**
        * @brief		Dereference operator
        * @details		Returns a reference to the element pointed to by the iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> Undefined if the iterator is not valid.\n
        * @returns		A reference to the element pointed by the iterator.
        */
		inline reference operator*() const
		{
			assert(m_buffer);
			return *(this->m_pointer.ptr);
		}

		/**
        * @brief		Dereference operator
        * @details		Returns a pointer to the element pointed to by the iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Unchanged.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> Undefined if the iterator is not valid.\n
        * @returns		A pointer to the element pointed by the iterator.
        */
		inline pointer operator&() const
		{
			assert(m_buffer);
			return this->m_pointer.ptr;
		}

		/**
        * @brief		Dereference operator
        * @details		Returns a pointer to the element pointed to by the iterator.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Unchanged.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> Undefined if the iterator is not valid.\n
        * @returns		A pointer to the element pointed by the iterator.
        */
		inline pointer operator->() const
		{
			assert(m_buffer);
			return this->m_pointer.ptr;
		}

		/**
        * @brief		Dereference iterator with offset
        * @details		Accesses the element located n positions away from the element currently pointed
        *				to by the iterator. If such an element does not exist, it causes <i>undefined
        *				behavior</i>.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Unchanged.\n
        *				<b> Data Races:</b> The object is accessed. Depending on the return type, the
        *				value returned may be used to access or modify elements.\n
        *				<b> Exception Safety:</b> Undefined behavior if <i>n</i> is out of range.\n
        * @param[in]	n Number of elements to offset. Member type difference_type is an alias of the
        *				base container's own difference type.
        * @returns		The element n positions away from the element currently pointed by the iterator.
        */
		inline reference operator[](difference_type n) const
		{
			assert(m_buffer);
			iterator tmp = *this + n;
			return *tmp;
		}

		/**
        * @brief		equality operator
        * @details		Two iterators are equal if they point to the same object and have the same
        *				parity value (i.e. one isn't wrapped around the buffer from the other).\n
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The container is accessed.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	rhs right-hand-side of the equation.
        * @returns		true if both iterators point to the same object.
        */
		inline bool operator==(const iterator& rhs) const noexcept
		{
			// memberwise ==
			assert(this->m_buffer == rhs.m_buffer);
			return ((this->m_pointer.ptr == rhs.m_pointer.ptr) && (this->m_pointer.parity == rhs.m_pointer.parity));
		}

		/**
        * @brief		inequality operator
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The container is accessed.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	rhs right-hand-side of the equation.
        * @returns		true if both iterators point to different objects.
        */
		inline bool operator!=(const iterator& rhs) const
		{
			// always define != in terms of == (unless you want awesomely undebuggable errors!)
			return !(operator==(rhs));
		}

		/**
        * @brief		Increment iterator position (prefix)
        * @details		Advances the iterator by 1 position.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Valid IFF the iterator is incrementable.\n
        *				<b> Data Races:</b> The object is modified.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @returns		A reference to the incremented iterator.
        */
		inline iterator& operator++() noexcept
		{
			return operator+=(1);
		}

		/**
        * @brief		Increment iterator position (postfix)
        * @details		Advances the iterator by 1 position.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Valid IFF the iterator is incrementable.\n
        *				<b> Data Races:</b> The object is modified.\n
        *				<b> Exception Safety:</b> Strong guarantee:  if the function throws an
        *					exception, there are no side effects.\n
        * @returns		A copy of the iterator before it was incremented.
        */
		inline iterator operator++(int)
		{
			auto tmp(*this);
			     operator++();
			return tmp;
		}

		/**
        * @brief		Advance iterator.
        * @details		Advances the iterator by n element positions.\n
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Results in undefined behavior if the element at
        *					position <i>n</i> does not exist.\n
        *				<b> Data Races:</b> The object is modified.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[]	n
        * @returns		circular_queueIterator&
        */
		inline iterator& operator+=(difference_type n) noexcept
		{
			assert(m_buffer && m_pointer.ptr);
			m_pointer = m_buffer->increment(m_pointer, n);

			return *this;
		}

		/**
        * @brief		Addition operator.
        * @details		Returns an iterator pointing to the element located n positions away from the
        *				element the iterator currently points to.\n
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Undefined behavior if the element <i>n</i> positions
        *				away is out of bounds.\n
        *				<b> Data Races:</b> The object is accessed but NOT modified.\n
        *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
        *					exception, there are no side effects.\n
        * @param[in]	n number of elements to offset.
        * @returns		An iterator pointing to the element n positions away.
        */
		inline iterator operator+(difference_type n) const noexcept
		{
			assert(m_buffer && m_pointer.ptr);

			queue_pointer p = m_buffer->increment(m_pointer, n);

			return iterator(m_buffer, p);
		}

		/**
        * @brief		Decrease iterator position (prefix)
        * @details		Decreases the iterator by one position.\n
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
        *					decrementable, otherwise no changes.\n
        *				<b> Data Races:</b> The object is modified.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @returns		A reference to an iterator pointing to the post-decrement iterator.
        */
		inline iterator& operator--() noexcept
		{
			return operator-=(1);
		}

		/**
        * @brief		Decrease iterator position (postfix)
        * @details		Decreases the iterator by one position.\n
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
        *					decrementable, otherwise no changes.\n
        *				<b> Data Races:</b> The object is accessed but NOT modified.\n
        *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
        *					exception, there are no side effects.\n
        * @returns		A iterator pointing to the pre-decremented element.
        */
		inline iterator operator--(int)
		{
			auto tmp(*this);
			     operator--();
			return tmp;
		}

		/**
        * @brief		Retrocede iterator
        * @details		Decreases the iterator by n element positions.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
        *					decrementable, otherwise no changes.\n
        *				<b> Data Races:</b> The object is modified.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	n Number of elements to offset. Member type difference_type is an alias of the
        *				base container's own difference type.\n
        * @returns		the iterator itself, decremented by n positions.
        */
		inline iterator& operator-=(difference_type n) noexcept
		{
			assert(m_buffer && m_pointer.ptr);
			m_pointer = m_buffer->decrement(m_pointer, n);

			return *this;
		}

		/**
        * @brief		subtraction operator
        * @details		Returns an iterator whose position is n elements before the current position
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
        *					decrementable, otherwise no changes.\n
        *				<b> Data Races:</b> the object is accessed but NOT modified.\n
        *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
        *					exception, there are no side effects.\n
        * @param[in]	n Number of elements to offset. Member type difference_type is an alias of the
        *				base container's own difference type.\n
        * @returns		An iterator decremented n positions from the current iterator position.
        */
		inline iterator operator-(difference_type n) const
		{
			assert(m_buffer && m_pointer.ptr);
			queue_pointer p = m_pointer;

			p = m_buffer->decrement(p, n);

			return iterator(m_buffer, p);
		}

		/**
        * @brief		subtraction operator
        * @details		Returns the distance between two iterators.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> Undefined behavior if the iterator is not
        *					decrementable, otherwise no changes.\n
        *				<b> Data Races:</b> the object is accessed but NOT modified.\n
        *				<b> Exception Safety:</b> Strong guarantee: if the constructor throws an
        *					exception, there are no side effects.\n
        * @param[in]	other iterator to determine distance from.\n
        * @returns		number of elements between the two iterators.
        */
		inline difference_type operator-(const iterator& other) const
		{
			assert(m_buffer);
			assert(m_buffer == other.m_buffer);
			assert(m_pointer.ptr && other.m_pointer.ptr);

			if (m_pointer.parity == other.m_pointer.parity)
			{
				return (m_pointer.ptr - other.m_pointer.ptr);
			}
			else
			{
				return (m_buffer->m_capacity - (other.m_pointer.ptr - m_pointer.ptr));
			}
		}

		/**
        * @brief		less-than operator
        * @details		Performs the appropriate comparison.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	rhs iterator for the right-hand side of the comparison.
        * @returns		true if this iterator is less than <i>rhs</i>.
        */
		inline bool operator<(const iterator& rhs) const noexcept
		{
			// check that the iterators are compatible
			assert(this->m_buffer == rhs.m_buffer);

			// it's possible the buffer is wrapped around, such that:
			//            T                     H
			// begin|--2--|---------------------|---------1---|end
			// this can be tested for by checking the parity bit. If the parity's match, they are not
			// wrapped. If the parity's don't match, they are wrapped.
			if (this->m_pointer.parity == rhs.m_pointer.parity)
			{
				return (this->m_pointer.ptr < rhs.m_pointer.ptr);
			}
			else
			{
				return this->m_pointer.ptr > rhs.m_pointer.ptr;
			}
		}

		/**
        * @brief		less-than-or-equal operator
        * @details		Performs the appropriate comparison.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	rhs iterator for the right-hand side of the comparison.
        * @returns		true if this iterator is less than or equal to <i>rhs</i>.
        */
		inline bool operator<=(const iterator& rhs) const
		{
			// define in terms of less than
			return (this->m_pointer.ptr == rhs.m_pointer.ptr || operator<(rhs));
		}

		/**
        * @brief		greater-than operator
        * @details		Performs the appropriate comparison.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	rhs iterator for the right-hand side of the comparison.
        * @returns		true if this iterator is greater than <i>rhs</i>.
        */
		inline bool operator>(const iterator& rhs) const
		{
			// define in terms of less than
			return !(operator<=(rhs));
		}

		/**
        * @brief		greater-than-or-equal operator
        * @details		Performs the appropriate comparison.
        *				<b> Complexity:</b> Constant.\n
        *				<b> Iterator Validity:</b> No changes.\n
        *				<b> Data Races:</b> The object is accessed.\n
        *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
        *					exceptions.\n
        * @param[in]	rhs iterator for the right-hand side of the comparison.
        * @returns		true if this iterator is greater than or equal to <i>rhs</i>.
        */
		inline bool operator>=(const iterator& rhs) const
		{
			// define in terms of less than
			return !(operator<(rhs));
		}

		/// Allow implicit conversion from iterator to const_iterator as required by STL
		operator const_iterator() const
		{
			return const_iterator(m_buffer, m_pointer);
		}

	protected:
		const circular_queue* m_buffer = nullptr;    //< pointer to the buffer object that this iterates on.
		queue_pointer         m_pointer;             ///< Iterator position.
	};

public:
	//////////////////////////////////////////////////////////////////////////
	//		#CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	/**
	* @{
    * @name     Constructors
    * @brief    Methods to construct, destruct, and assign the container.
    */

	/**
     * @brief		Constructor
     * @details
     *				<b> Complexity:</b> O(N) with buffer capacity (memory allocation).\n
     *				<b> Iterator Validity:</b> N/A.\n
     *				<b> Data Races:</b> N/A.\n
     *				<b> Exception Safety:</b> Throws std::bad_alloc if allocation fails. In this
     *				case, no object is created.\n
     * @param[in]	capacity number of elements that can be stored in the queue.
     * @param[in]	alloc Allocator object. The container keeps and uses an internal copy of this
     *				allocator.
     */
	explicit circular_queue(size_type capacity, const allocator_type& alloc = allocator_type())
	    : m_alloc(alloc)
	    , m_capacity(capacity)
	    , m_head(nullptr, false)
	    , m_tail(nullptr, false)
	{
		if (m_capacity == 0)
		{
			throw std::bad_alloc();
		}

		// allocate raw memory near this class. No point in default constructing anything, especially since this buffer
		// may be HUGE.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);
		}
		catch (...)
		{
			// Since constructor failed, no object will be created. Memory wasn't allocated, so no
			// cleanup is required.
			// transparent exceptions
			throw std::bad_alloc();
		}

		m_head.ptr = m_tail.ptr = m_data;
	}

	/**
     * @brief		fill constructor
     * @details		Creates an array of size <i>capacity</i> and fills it with copies of <i>val</i>.\n
     *				<b> Complexity:</b> O(N) with size.\n
     *				<b> Iterator Validity:</b> N/A.\n
     *				<b> Data Races:</b> N/A.\n
     *				<b> Exception Safety:</b> Throws std::bad_alloc if allocation fails. In this
     *				case, no object is created.\n
     * @param[in]	val this value will be copied into all the container elements.
     * @param[in]	capacity size of the container.
     * @param[in]	alloc Allocator object. The container keeps and uses an internal copy of this
     *				allocator.
     */
	explicit circular_queue(size_type capacity, const value_type& val, const allocator_type& alloc = allocator_type())
	    : m_alloc(alloc)
	    , m_capacity(capacity)
	    , m_head(nullptr, false)
	    , m_tail(nullptr, false)
	{
		if (m_capacity == 0)
		{
			throw std::bad_alloc();
		}

		// allocate raw memory near this class.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);
		}
		catch (...)
		{
			// Since constructor failed, no object will be created. Memory wasn't allocated, so no
			// cleanup is required.
			// transparent exceptions
			throw std::bad_alloc();
		}

		m_head.ptr = m_tail.ptr = m_data;

		// fill class by copy-constructing from val
		try
		{
			// fill container by copy constructing value
			for (int i = 0; i < m_capacity; ++i)
			{
				emplace_back(val);
			}
		}
		catch (...)
		{
			// if this fails, cleanup what's been done and then re-throw
			this->~circular_queue();
			throw;
		}
	}

	/**
     * @brief		Range Constructor
     * @details		Creates a container by copying the objects in the range of the InputIterators [first, last).
     *				The size and capacity of the container will be equal to the size of the range.\n
     *				<b> Complexity:</b> O(N) with range.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> N/A.\n
     *				<b> Exception Safety:</b> Throws std::bad_alloc if allocation fails. In this
     *				case, no object is created.\n
     * @param[in]	first iterator to the beginning of the range to copy.
     * @param[in]	last iterator to the end (one past the last element) of the range to copy.
     * @param[in]	alloc Allocator object. The container keeps and uses an internal copy of this
     *				allocator.
     */
	template<class InputIterator>
	explicit circular_queue(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type(), typename std::enable_if<!std::is_integral<InputIterator>::value>::type* = 0)
	    : m_alloc(alloc)
	    , m_capacity(last - first)
	    , m_head(nullptr, false)
	    , m_tail(nullptr, false)
	{
		static_assert(std::is_convertible<value_type, typename std::iterator_traits<InputIterator>::value_type>::value,
		              "InputIterator::value_type is not compatible with this container's value_type");

		if (m_capacity == 0)
		{
			throw std::bad_alloc();
		}

		// allocate raw memory near this class.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);
		}
		catch (...)
		{
			// Since constructor failed, no object will be created. Memory wasn't allocated, so no
			// cleanup is required.
			// transparent exceptions
			throw std::bad_alloc();
		}
		m_head.ptr = m_tail.ptr = m_data;

		// fill class by copy-constructing from val
		try
		{
			// fill container by copy constructing from the iterators
			for (auto itr = first; itr != last; ++itr)
			{
				emplace_back(*itr);
			}
		}
		catch (...)
		{
			// if this fails, cleanup what's been done and then re-throw
			this->~circular_queue();
			throw;
		}
	}

	/**
     * @brief		construct from initializer list
     * @details		constructs the circular_queue by copying elements from the initializer_list. The
     *				size and capacity of the queue will be equal to the initializer_list size.\n
     *				<b> Complexity:</b> Linear with size of <i>il</i> (constructions).\n
     *				<b> Iterator Validity:</b> N/A.\n
     *				<b> Data Races:</b> N/A.\n
     *				<b> Exception Safety:</b> Throws std::bad_alloc if allocation fails. In this
     *				case, no object is created.\n
     * @param[in]	il initializer_list object.
     * @param[in]	alloc Allocator object. The container keeps and uses an internal copy of this
     */
	circular_queue(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
	    : m_alloc(alloc)
	    , m_capacity(il.size())
	    , m_head(nullptr, false)
	    , m_tail(nullptr, false)
	{
		if (m_capacity == 0)
		{
			throw std::bad_alloc();
		}

		// allocate raw memory near this class.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);
		}
		catch (...)
		{
			// Since constructor failed, no object will be created. Memory wasn't allocated, so no
			// cleanup is required.
			// transparent exceptions
			throw std::bad_alloc();
		}

		m_head.ptr = m_tail.ptr = m_data;

		// fill class by copy-constructing from val
		try
		{
			// fill container by copy constructing from the iterators
			for (auto itr = il.begin(); itr != il.end(); ++itr)
			{
				emplace_back(*itr);
			}
		}
		catch (...)
		{
			// if this fails, cleanup what's been done and then re-throw
			this->~circular_queue();
			throw;
		}
	}

	/**
     * @brief		Copy Constructor
     * @details		Creates a deep copy of another circular_queue.\n
     *				<b> Complexity:</b> O(N) with buffer capacity (memory allocation/copy).\n
     *				<b> Iterator Validity:</b> N/A.\n
     *				<b> Data Races:</b> N/A.\n
     *				<b> Exception Safety:</b> Throws std::bad_alloc if allocation fails. In this
     *				case, no object is created.\n
     * @param[in]	other buffer to be copied
     */
	circular_queue(const circular_queue& other)
	    : m_alloc(std::allocator_traits<decltype(other.m_alloc)>::select_on_container_copy_construction(other.m_alloc))
	    , m_capacity(other.m_capacity)
	    , m_head(other.m_head)
	    , m_tail(other.m_tail)
	{
		// allocate raw memory near this class. No point in default constructing anything, especially since this buffer
		// may be HUGE.
		m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);

		// correct head/tail pointers
		m_head.ptr = m_data + (other.m_head.ptr - other.m_data);
		m_tail.ptr = m_data + (other.m_tail.ptr - other.m_data);

		// Deep copy the other data - need to copy construct each individual element. No memcpy!!
		for (const_iterator itr = this->begin(), otherItr = other.begin(); otherItr != other.end(); ++itr, ++otherItr)
		{
			// copy construct
			std::allocator_traits<Alloc>::construct(m_alloc, &itr, *otherItr);
		}
	}

	/**
     * @brief		move constructor
     * @details		constructs a circular_queue and obtains its resources by moving them from another
     *				instance.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> All iterators to the other container are invalidated.\n
     *				<b> Data Races:</b> Container <i>other</i> is modified.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions.\n
     * @param[in]	other circular_queue to be moved.
     */
	circular_queue(circular_queue&& other) noexcept
	    : m_alloc(std::move(other.m_alloc))
	    , m_capacity(std::move(other.m_capacity))
	    , m_head(std::move(other.m_head))
	    , m_tail(std::move(other.m_tail))
	{
		// self assignment could be bad.
		if (this != &other)
		{
			// steal allocated storage from other class
			m_data              = other.m_data;
			other.m_data        = nullptr;
			other.m_capacity    = 0;
			other.m_head.ptr    = nullptr;
			other.m_tail.ptr    = nullptr;
			other.m_head.parity = false;
			other.m_tail.parity = false;
		}
	}

	/** @} */    // END OF CONSTRUCTORS

	//////////////////////////////////////////////////////////////////////////
	//		#DESTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	/**
     * @{
     * @name    Destructor
     */

	/**
    * @brief		destructor
    * @details		Yeah, you know what this is.
    */
	virtual ~circular_queue()
	{
		// destroy contents
		clear();
		// deallocate memory
		m_alloc.deallocate(m_data, m_capacity);
	}

	/** @} */    // end of destructors

	//////////////////////////////////////////////////////////////////////////
	//		#OPERATORS
	//////////////////////////////////////////////////////////////////////////

	/**
     * @{
     * @name	Operators
     */

	/**
     * @brief		Assign content
     * @details		copies all the elements from <i>other</i> into the container (with <i>other</i>
     *				preserving its contents).\n
     *				<b> Complexity:</b> Linear in initial and final sizes (destructions, copy constructions).\n
     *				<b> Iterator Validity:</b> All iterators, references and pointers related to
     *					this container before the call are invalidated.\n
     *				<b> Data Races:</b> All copied elements are accessed. The container and all its
     *					elements are modified.\n
     *				<b> Exception Safety:</b> Basic guarantee: if an exception is thrown, the
     *					container is in a valid state. If allocator_traits::construct is not
     *					supported with the appropriate arguments for the element constructions, or
     *					if value_type is not copy assignable, it causes undefined behavior.\n
     * @param[in]	other container to copy
     * @sa
     * @returns		*this
     */
	circular_queue& operator=(const circular_queue& other)
	{
		// destroy and deallocate current container first
		this->~circular_queue();

		if (std::allocator_traits<decltype(other.m_alloc)>::propagate_on_container_copy_assignment::value)
		{
			m_alloc = other.m_alloc;
		}

		m_capacity    = other.m_capacity;
		m_head.parity = other.m_head.parity;
		m_tail.parity = other.m_tail.parity;

		// allocate raw memory near this class. No point in default constructing anything, especially since this buffer
		// may be HUGE.
		m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);

		// correct head/tail pointers
		m_head.ptr = m_data + (other.m_head.ptr - other.m_data);
		m_tail.ptr = m_data + (other.m_tail.ptr - other.m_data);

		// Deep copy the other data - need to copy construct each individual element. No memcpy!!
		for (const_iterator itr = this->begin(), otherItr = other.begin(); otherItr != other.end(); ++itr, ++otherItr)
		{
			// copy construct
			std::allocator_traits<Alloc>::construct(m_alloc, &itr, *otherItr);
		}

		return *this;
	}

	/**
     * @brief		Assign content
     * @details		moves all the elements from <i>other</i> into the container (with <i>other</i>
     *				in an unspecified but valid state).\n
     *				<b> Complexity:</b> Linear in Size (destructions).\n
     *				<b> Iterator Validity:</b> All iterators, references and pointers related to
     *					this container before the call are invalidated.\n
     *				<b> Data Races:</b> All copied elements are accessed. The container and all its
     *					elements are modified.\n
     *				<b> Exception Safety:</b> Basic guarantee: if an exception is thrown, the
     *					container is in a valid state. If allocator_traits::construct is not
     *					supported with the appropriate arguments for the element constructions, or
     *					if value_type is not move assignable, it causes undefined behavior.\n
     * @param[in]	other container to move
     * @sa			assign()
     * @returns		*this
     */
	circular_queue& operator=(circular_queue&& other) noexcept
	{
		// destroy and deallocate current container first
		this->~circular_queue();

		if (std::allocator_traits<decltype(other.m_alloc)>::propagate_on_container_move_assignment::value)
		{
			m_alloc = std::move(other.m_alloc);
		}

		m_capacity = std::move(other.m_capacity);
		m_head     = std::move(other.m_head);
		m_tail     = std::move(other.m_tail);

		// steal allocated storage from other class
		m_data = other.m_data;

		// set everything in the other class to null
		other.m_data        = nullptr;
		other.m_capacity    = 0;
		other.m_head.ptr    = nullptr;
		other.m_tail.ptr    = nullptr;
		other.m_head.parity = false;
		other.m_tail.parity = false;

		return *this;
	}

	/**
     * @brief		Assign content
     * @details		Assigns new contents to the container, replacing its current contents, and
     *				modifying its size accordingly. The new contents are copies of the values passed
     *				as initializer list, in the same order.\n
     *				<b> Complexity:</b> Linear in initial and final sizes (destructions, constructions).\n
     *				<b> Iterator Validity:</b> All iterators, references and pointers related to
     *					this container before the call are invalidated.
     *				<b> Data Races:</b> All copied elements are accessed. The container and all its
     *					elements are modified.\n
     *				<b> Exception Safety:</b> Basic guarantee: if an exception is thrown, the
     *					container is in a valid state. If allocator_traits::construct is not
     *					supported with the appropriate arguments for the element constructions, or
     *					if value_type is not move assignable, it causes undefined behavior.\n
     * @param[in]	il	initializer list object.
     * @returns		circular_queue&
     */
	circular_queue& operator=(std::initializer_list<value_type> il)
	{
		assign(std::forward<std::initializer_list<value_type>>(il));
		return *this;
	}

	/**
     * @brief		Access element
     * @details		Returns a reference to the element at position n in the container. A similar
     *				member function, at(), has the same behavior as this operator function, except
     *				that at() is bound-checked and signals if the requested position is out of range
     *				by throwing an out_of_range exception. Portable programs should never call this
     *				function with an argument n that is out of range, since this causes undefined
     *				behavior.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *				non-const versions modify the container). The reference returned can be used to
     *				access or modify elements. Concurrently accessing or modifying different
     *				elements is safe.\n
     *				<b> Exception Safety:</b> If the container size is greater than n, the function
     *				never throws exceptions (no-throw guarantee). Otherwise, the behavior is
     *				undefined.\n
     * @param[in]	n Position of an element in the container. Notice that the first element has a
     *				position of 0 (not 1). Member type size_type is an unsigned integral type.
     * @returns		The element at the specified position in the circular_queue.
     */
	reference operator[](size_type n)
	{
		assert(n < m_capacity);
		return *(m_data + ((m_head.ptr - m_data + n) % m_capacity));
	}

	/// @overload
	const_reference operator[](size_type n) const
	{
		assert(n < m_capacity);
		return *(m_data + ((m_head.ptr - m_data + n) % m_capacity));
	}

	/** @} */    // end of operators

	//////////////////////////////////////////////////////////////////////////
	//		#ITERATORS
	//////////////////////////////////////////////////////////////////////////

	/**
    * @{
    * @name		Iterators
    * @brief	Methods to construct iterators to the container.
    */

	/**
     * @brief		Returns iterator to beginning
     * @details		Returns an iterator to the head (first element) in the container.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *					non-const versions modify the container). No contained elements are accessed by
     *					the call, but the iterator returned can be used to access or modify elements.
     *					Concurrently accessing or modifying different elements is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions. The copy construction or assignment of the returned iterator is also
     *					guaranteed to never throw.\n
     * @returns		An iterator to the beginning of the sequence container.
     */
	iterator begin() noexcept
	{
		return iterator(this, m_head);
	}

	/// @overload
	const_iterator begin() const noexcept
	{
		return const_iterator(this, m_head);
	}

	/**
     * @brief		Returns iterator to end
     * @details		Returns an iterator to the tail (last element) in the container.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *					non-const versions modify the container). No contained elements are accessed by
     *					the call, but the iterator returned can be used to access or modify elements.
     *					Concurrently accessing or modifying different elements is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions. The copy construction or assignment of the returned iterator is also
     *					guaranteed to never throw.\n
     * @returns		An iterator to the end of the sequence container.
     */
	iterator end() noexcept
	{
		return iterator(this, m_tail);
	}

	/// @overload
	const_iterator end() const noexcept
	{
		return const_iterator(this, m_tail);
	}

	/**
	 * @function    rbegin
    * @brief		Returns reverse iterator to beginning
    * @details		Returns a reverse iterator to the reverse beginning (last element) in the
    *				container. rbegin points to the element right before the one that would be
    *				pointed to by end().\n
    *				<b> Complexity:</b> Constant.\n
    *				<b> Iterator Validity:</b> No changes.\n
    *				<b> Data Races:</b> The container is accessed (neither the const nor the
    *					non-const versions modify the container). No contained elements are accessed by
    *					the call, but the iterator returned can be used to access or modify elements.
    *					Concurrently accessing or modifying different elements is safe.\n
    *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
    *					exceptions. The copy construction or assignment of the returned iterator is also
    *					guaranteed to never throw.\n
    * @returns		A reverse iterator to the beginning of the sequence container.
    */
	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	/// @overload
	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	/**
    * @brief		Returns reverse iterator to end
    * @details		Returns a reverse iterator pointing to the theoretical element preceding the
    *				first element in the container (which is considered its reverse end).\n
    *				<b> Complexity:</b> Constant.\n
    *				<b> Iterator Validity:</b> No changes.\n
    *				<b> Data Races:</b> The container is accessed (neither the const nor the
    *					non-const versions modify the container). No contained elements are accessed by
    *					the call, but the iterator returned can be used to access or modify elements.
    *					Concurrently accessing or modifying different elements is safe.\n
    *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
    *					exceptions. The copy construction or assignment of the returned iterator is also
    *					guaranteed to never throw.\n
    * @returns		A reverse iterator to the end of the sequence container.
    */
	reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	/// @overload
	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

	/**
     * @brief		Returns constant iterator to beginning
     * @details		Returns a constant iterator to the head (first element) in the container. A
     *				const_iterator is an iterator that points to const content. This iterator can be
     *				increased and decreased (unless it is itself also const), just like the iterator
     *				returned by begin(), but it cannot be used to modify the contents it points to,
     *				even if the container object is not itself const.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *					non-const versions modify the container). No contained elements are accessed by
     *					the call, but the iterator returned can be used to access or modify elements.
     *					Concurrently accessing or modifying different elements is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions. The copy construction or assignment of the returned iterator is also
     *					guaranteed to never throw.\n
     * @returns		A constant iterator to the beginning of the sequence container.
     */
	const_iterator cbegin() const noexcept
	{
		return const_iterator(this, m_head);
	}

	/**
     * @brief		Returns constant iterator to end
     * @details		Returns a constant iterator to the tail (last element) in the container.A
     *				const_iterator is an iterator that points to const content. This iterator can be
     *				increased and decreased (unless it is itself also const), just like the iterator
     *				returned by begin(), but it cannot be used to modify the contents it points to,
     *				even if the container object is not itself const.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *					non-const versions modify the container). No contained elements are accessed by
     *					the call, but the iterator returned can be used to access or modify elements.
     *					Concurrently accessing or modifying different elements is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions. The copy construction or assignment of the returned iterator is also
     *					guaranteed to never throw.\n
     * @returns		A constant iterator to the end of the sequence container.
     */
	const_iterator cend() const noexcept
	{
		return const_iterator(this, m_tail);
	}

	/**
    * @brief		Returns constant iterator to beginning
    * @details		Returns a reverse iterator to the reverse beginning (last element) in the
    *				container. crbegin points to the element right before the one that would be
    *				pointed to by end(). A const_reverse_iterator is an iterator that points to const content.
    *				This iterator can be increased and decreased (unless it is itself also const),
    *				just like the iterator returned by begin(), but it cannot be used to modify the
    *				contents it points to, even if the container object is not itself const.\n
    *				<b> Complexity:</b> Constant.\n
    *				<b> Iterator Validity:</b> No changes.\n
    *				<b> Data Races:</b> The container is accessed (neither the const nor the
    *					non-const versions modify the container). No contained elements are accessed by
    *					the call, but the iterator returned can be used to access or modify elements.
    *					Concurrently accessing or modifying different elements is safe.\n
    *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
    *					exceptions. The copy construction or assignment of the returned iterator is also
    *					guaranteed to never throw.\n
    * @returns		A constant iterator to the beginning of the sequence container.
    */
	const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	/**
    * @brief		Returns constant iterator to end
    * @details		Returns a reverse iterator pointing to the theoretical element preceding the
    *				first element in the container (which is considered its reverse end). A
    *				const_reverse_iterator is an iterator that points to const content. This iterator can be
    *				increased and decreased (unless it is itself also const), just like the iterator
    *				returned by begin(), but it cannot be used to modify the contents it points to,
    *				even if the object is not itself const.\n
    *				<b> Complexity:</b> Constant.\n
    *				<b> Iterator Validity:</b> No changes.\n
    *				<b> Data Races:</b> The container is accessed (neither the const nor the
    *					non-const versions modify the container). No contained elements are accessed by
    *					the call, but the iterator returned can be used to access or modify elements.
    *					Concurrently accessing or modifying different elements is safe.\n
    *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
    *					exceptions. The copy construction or assignment of the returned iterator is also
    *					guaranteed to never throw.\n
    * @returns		A constant iterator to the end of the sequence container.
    */
	const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

	/** @} */    // END OF ITERATORS

	//////////////////////////////////////////////////////////////////////////
	//		#CAPACITY
	//////////////////////////////////////////////////////////////////////////

	/**
    * @{
    * @name		Capacity
    * @brief	Methods to determine the capacity of the container
    */

	/**
     * @brief		Return size
     * @details		Returns the number of elements in the container.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed. No contained elements are accessed:
     *					concurrently accessing or modifying them is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions..\n
     * @returns		The number of elements in the container. Member type size_type is an unsigned
     *				integral type.
     */
	[[nodiscard]] inline size_type size() const noexcept
	{
		if (m_head.parity == m_tail.parity)
		{
			// difference between head and tail represents amount used.
			return static_cast<size_type>(m_tail.ptr - m_head.ptr);
		}
		else
		{
			// difference between head and tail represents amount unused.
			return (m_capacity - static_cast<size_type>(m_head.ptr - m_tail.ptr));
		}
	}

	/**
    * @brief		Get buffer capacity
    * @details		Returns the allocated size of the buffer. For the amount of the buffer in use,
    *				see size().\n
    *				<b> Complexity:</b> Constant.\n
    *				<b> Iterator Validity:</b> No changes.\n
    *				<b> Data Races:</b> The container object is accessed.\n
    *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
    *					exceptions.\n
    * @sa			size()
    * @returns		capacity of the buffer in terms of how many objects of <i>value_type</i> it can contain.
    */
	[[nodiscard]] size_type capacity() const noexcept
	{
		return m_capacity;
	}

	/**
     * @brief		Return maximum capacity
     * @details		Returns the maximum number of elements that the container can hold. This
     *				is the maximum potential capacity the container can reach due to known system or
     *				library implementation limitations, but the container is by no means guaranteed
     *				to be able to reach that capacity: it can still fail to allocate storage at any
     *				point before that capacity is reached.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed, but no contained elements are
     *				accessed. Concurrently accessing or modifying them is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
     *					exceptions.\n
     * @returns		Maximum number of elements of type <i>value_type</i> that can be stored in the
     *				container.
     */
	[[nodiscard]] size_type max_size() const noexcept
	{
		return std::allocator_traits<Alloc>::max_size(m_alloc);
	}

	/**
     * @brief		Test whether container is empty
     * @details		Returns whether the container is empty (i.e. whether its size is 0).\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container object is accessed, but no elements are accessed.\n
     *				<b> Exception Safety:</b> No-throw guarantee: This function never throws
     *					exceptions.\n
     * @returns		true if the buffer is empty, false otherwise.
     */
	[[nodiscard]] inline bool empty() const noexcept
	{
		return (size() == 0);
	}

	/**
     * @brief		Test whether container is full
     * @details		Returns whether the container is empty (i.e. whether its size is equal to it's capacity).\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container object is accessed, but no elements are accessed.\n
     *				<b> Exception Safety:</b> .\n
     * @returns		bool
     */
	[[nodiscard]] inline bool full() const noexcept
	{
		return (size() >= m_capacity);
	}

	/**
     * @brief		Request a change in capacity
     * @details		Requests that the vector capacity be at least enough to contain n elements. If
     *				n is greater than the current capacity, the function causes the container to
     *				reallocate its storage increasing its capacity to n. In all other cases, the
     *				function call does not cause a reallocation and the vector capacity is not
     *				affected.\n
     *				<b> Complexity:</b> If a reallocation happens, linear in size at most..\n
     *				<b> Iterator Validity:</b> If a reallocation happens, all iterators, pointers
     *					and references related to the container are invalidated. Otherwise, they all
     *					keep referring to the same elements they were referring to before the call.\n
     *				<b> Data Races:</b> If a reallocation happens, the container and all its
     *					contained elements are modified.\n
     *				<b> Exception Safety:</b> If no reallocations happen or if the type of the
     *					elements has either a non-throwing move constructor or a copy constructor,
     *					there are no changes in the container in case of exception (strong guarantee).
     *					Otherwise, the container is guaranteed to end in a valid state (basic guarantee).
     *					Under the basic guarantee, if this function throws, it's possible that all the
     *					container contents will be destroyed as a side-effect.
     *					The function throws bad_alloc if it fails to allocate the new size.\n
     * @param[in]	n Minimum capacity for the container.
     */
	void reserve(size_type n)
	{
		if (n <= size())
		{
			return;
		}

		reallocate(n);
	}

	/**
     * @brief		Change size
     * @details		Changes <i>both</i> the size and capacity of the container, resizing the
     *				container so that it contains n elements. If n is smaller than the current
     *				container size, the content is reduced to its first n elements, removing those
     *				beyond (and destroying them). If n is greater than the current container size,
     *				the content is expanded by inserting at the end as many elements as needed to
     *				reach a size of n. If val is specified, the new elements are initialized as
     *				copies of val, otherwise, they are value-initialized. Regardless of the value of n
     *				(except n == size()), an automatic reallocation of the allocated
     *				storage space takes place. Notice that this function changes the actual content
     *				of the container by inserting or erasing elements from it.\n
     *				<b> Complexity:</b> Linear on the number of elements inserted/erased
     *					(constructions/destructions). If a reallocation happens, the reallocation
     *					is itself up to linear in the entire container size.\n
     *				<b> Iterator Validity:</b> Resizing causes reallocation, and so all iterators, pointers and references
     *					related to this container are also invalidated.\n
     *				<b> Data Races:</b> The container is modified. If a reallocation happens, all
     *					contained elements are modified.\n
     *				<b> Exception Safety:</b> If n is less than or equal to the size of the container,
     *					the function never throws exceptions (no-throw guarantee). If n is greater
     *					and a reallocation happens, there are no changes in the container in case of
     *					exception (strong guarantee) if the type of the elements is either copyable
     *					or no-throw movable. Otherwise, if an exception is thrown, the container is
     *					left with a valid state (basic guarantee).\n
     * @param[in]	n	New container size, expressed in number of elements. Member type <i>size_type</i>
     *					is an unsigned integral type.
     * @param[in]	val	Object whose content is copied to the added elements in case that n is
     *					greater than the current container size. If not specified, the default
     *					constructor is used instead.
     */
	void resize(size_type n, const value_type& val = value_type())
	{
		// just because they tell you to jump off a bridge...
		if (n == size())
		{
			return;
		}
		// if we're making it bigger, it's just a reserve + copy fill
		else if (n > size())
		{
			// reserve moves all the existing elements in
			reserve(n);
			// fill the rest with val copies
			while (size() < capacity())
			{
				emplace_back(val);
			}
		}
		// if we're making it smaller, pop the back until it's the right size, then deallocate the
		// extra.
		else
		{
			while (size() > n)
			{
				pop_back();
			}
			shrink_to_fit();
		}
	}

	/**
     * @brief		Shrink container to fit
     * @details		Requests the container to reduce its capacity to fit its size. Unlike vector,
     *				the request is binding. This will never cause a reallocation, and has no effect
     *				on the size or element contents.\n
     *				<b> Complexity:</b> At most, linear in container size..\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references related to the
     *					container are invalidated.\n
     *				<b> Data Races:</b> The container is modified. All contained objects are moved.\n
     *				<b> Exception Safety:</b> If the type of the elements is either copyable or
     *					no-throw movable, there are no changes in the container in case of exception
     *					(strong guarantee). Otherwise, if an exception is thrown, the container is
     *					left with a valid state (basic guarantee).\n
     */
	void shrink_to_fit()
	{
		if (capacity() == size())
		{
			return;
		}

		reallocate(size());
	}

	/** @} */    // END OF CAPACITY

	//////////////////////////////////////////////////////////////////////////
	//		#ELEMENT ACCESS
	//////////////////////////////////////////////////////////////////////////

	/**
     * @{
     * @name	Element Access
     * @brief	Members that provide access to individual elements contained withing the circular_queue
     */

	/**
     * @brief		Access element
     * @details		Returns a reference to the element at position n in the container object.
     *				The function automatically checks whether n is within the bounds of valid
     *				elements in the container, throwing an out_of_range exception if it is not
     *				(i.e., if n is greater or equal than its size). This is in contrast with member
     *				operator[], that does not check against bounds.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> .\n
     *				<b> Data Races:</b> .\n
     *				<b> Exception Safety:</b> .\n
     * @param[in]	n Position of an element in the container. If this is greater than the container
     *				size, an exception of type out_of_range is thrown. Notice that the first element
     *				has a position of 0 (not 1). Member type size_type is an unsigned integral type.
     * @returns		The element at the specified position in the container. If the circular_queue
     *				object is const-qualified, the function returns a const_reference. Otherwise,
     *				it returns a reference.
     */
	reference at(size_type n)
	{
		if (n >= size())
		{
			throw std::out_of_range("index larger than container size");
		}

		return *(m_data + ((m_head.ptr - m_data + n) % m_capacity));
	}

	/// @overload
	const_reference at(size_type n) const
	{
		if (n >= size())
		{
			throw std::out_of_range("index larger than container size");
		}

		return *(m_data + ((m_head.ptr - m_data + n) % m_capacity));
	}

	/**
     * @brief		Access first element
     * @details		Returns a reference to the first element in the circular_queue. Unlike member
     *				begin(), which returns an iterator to this same element, this function returns
     *				a direct reference. Calling this function on an empty container causes undefined
     *				behavior.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *					non-const versions modify the container). The reference returned can be used
     *					to access or modify elements. Concurrently accessing or modifying different
     *					elements is safe.\n
     *				<b> Exception Safety:</b> If the container is not empty, the function never
     *					throws exceptions (no-throw guarantee). Otherwise, it causes undefined
     *					behavior.\n
     * @returns		A reference to the first element in the circular_queue.
     */
	reference front() noexcept
	{
		return *m_head.ptr;
	}

	/// @overload
	const_reference front() const noexcept
	{
		return *m_head.ptr;
	}

	/**
     * @brief		Access last element
     * @details		Returns a reference to the last element in the circular_queue. Unlike member
     *				end(), which returns an iterator just past this element, this function returns
     *				a direct reference. Calling this function on an empty container causes undefined
     *				behavior.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed (neither the const nor the
     *					non-const versions modify the container). The reference returned can be used
     *					to access or modify elements. Concurrently accessing or modifying different
     *					elements is safe.\n
     *				<b> Exception Safety:</b> If the container is not empty, the function never
     *					throws exceptions (no-throw guarantee). Otherwise, it causes undefined
     *					behavior.\n
     * @returns		A reference to the last element in the circular_queue.
     */
	reference back()
	{
		queue_pointer p = decrement(m_tail, 1);
		return *p.ptr;
	}

	/// @overload
	const_reference back() const
	{
		queue_pointer p = decrement(m_tail, 1);
		return *p.ptr;
	}

	/** @} */    // end of element_access

	//////////////////////////////////////////////////////////////////////////
	//		#MODIFIERS
	//////////////////////////////////////////////////////////////////////////

	/**
     * @{
     * @name	Modifiers
     * @brief	Member functions that modify the container.
     */

	/**
     * @brief		Assign container content range
     * @details		Assigns new contents to the container, replacing its current contents, and
     *				modifying its size and capacity accordingly.The new contents are elements
     *				constructed from each of the elements in the range
     *				between first and last, in the same order. The range used is [first, last).\n
     *				<b> Complexity:</b> Linear in initial and final sizes (destructions, constructions).\n
     *				<b> Iterator Validity:</b> All iterators are invalidated.\n
     *				<b> Data Races:</b> The container and all elements are modified. All copied
     *					elements are accessed.\n
     *				<b> Exception Safety:</b> Basic guarantee: if an exception is thrown, the
     *					container is in a valid state. If allocator_traits::construct is not
     *					supported with the appropriate arguments for the element constructions, or
     *					if the range specified by [first,last) is not valid, it causes undefined
     *					behavior. Throws std::length_error if first == last.\n
     * @param[in]	first beginning of the range to copy.
     * @param[in]	last end (exclusive) of the range to be copied.
     * @sa			operator=
     */
	template<class InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
		static_assert(std::is_convertible<value_type, typename std::iterator_traits<InputIterator>::value_type>::value,
		              "InputIterator::value_type is not compatible with this container's value_type");

		// check that there is a valid range
		difference_type distance = last - first;
		if (distance == 0)
		{
			throw std::length_error("Cannot assign capacity of 0");
		}

		// delete and deallocate current contents
		this->~circular_queue();

		// set new capacity
		m_capacity = distance;

		// allocate raw memory near this class.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);
		}
		catch (...)
		{
			// transparent exceptions
			// leave container in valid state
			m_capacity = 0;
			m_head.ptr = m_tail.ptr = m_data = nullptr;
			m_head.parity = m_tail.parity = false;
			throw std::bad_alloc();
		}

		m_head.ptr = m_tail.ptr = m_data;
		m_head.parity = m_tail.parity = false;

		// fill class
		try
		{
			// fill container by copy constructing from the iterators
			for (auto itr = first; itr != last; ++itr)
			{
				emplace_back(*itr);
			}
		}
		catch (...)
		{
			// if this fails, delete whats been made
			clear();
			throw;
		}
	}

	/**
     * @brief		Assign container content by fill
     * @details		Assigns new contents to the container, replacing its current contents,
     *				and modifying its size and capacity accordingly. The new contents are n elements, each
     *				initialized to a copy of val.\n
     *				<b> Complexity:</b> Linear in initial and final sizes (destructions, constructions).\n
     *				<b> Iterator Validity:</b> All iterators, pointers, and references are invalidated.\n
     *				<b> Data Races:</b> The container and all elements are modified. All copied
     *					elements are accessed.\n
     *				<b> Exception Safety:</b> Basic guarantee: if an exception is thrown, the
     *					container is in a valid state. If allocator_traits::construct is not
     *					supported with the appropriate arguments for the element constructions, or
     *					if the range specified by [first,last) is not valid, it causes undefined
     *					behavior. Throws std::length_error if first == last.\n
     * @param[in]	n	new container size
     * @param[in]	val value to be copied into each element of the container.
     */
	void assign(size_type n, const value_type& val)
	{
		// check that there is a valid range
		if (n == 0)
		{
			throw std::length_error("Cannot assign capacity of 0");
		}

		// delete and deallocate current contents
		this->~circular_queue();

		// set new capacity
		m_capacity = n;

		// allocate raw memory near this class.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity > n ? m_capacity : n, this);
		}
		catch (...)
		{
			// transparent exceptions
			// leave container in valid state
			m_capacity = 0;
			m_head.ptr = m_tail.ptr = m_data = nullptr;
			m_head.parity = m_tail.parity = false;
			throw std::bad_alloc();
		}

		m_head.ptr = m_tail.ptr = m_data;
		m_head.parity = m_tail.parity = false;

		// fill class
		try
		{
			// fill container by copy constructing val
			for (size_type i = 0; i < n; ++i)
			{
				emplace_back(val);
			}
		}
		catch (...)
		{
			// if this fails, cleanup what's been done and then re-throw
			// if this fails, delete whats been made
			clear();
			throw;
		}
	}

	/**
     * @brief		Assign container contents from initializer list
     * @details		Assigns new contents to the  container, replacing its current contents, and
     *				modifying its size and capacity accordingly. The new contents are copies of the
     *				values passed as initializer list, in the same order.\n
     *				<b> Complexity:</b> Linear in initial and final sizes (destructions, constructions).\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references related to this container are invalidated.\n
     *				<b> Data Races:</b> All copied elements are accessed. The container is modified.
     *					All contained elements are modified.\n
     *				<b> Exception Safety:</b> Basic guarantee: if an exception is thrown, the
     *					container is in a valid state. If allocator_traits::construct is not supported
     *					with the appropriate arguments for the element constructions it causes
     *					undefined behavior. If the initializer list is empty, std::length_error is
     *					thrown.\n
     * @param[in]	il An initializer_list object. The compiler will automatically construct such
     *				objects from initializer list declarators. Member type value_type is the type of
     *				the elements in the container, defined in circular_queue as an alias of its first
     *				template parameter (T).
     * @sa			operator=
     */
	void assign(std::initializer_list<value_type> il)
	{
		// check that there is a valid range
		if (il.size() == 0)
		{
			throw std::length_error("Cannot assign capacity of 0");
		}

		// delete and deallocate current contents
		this->~circular_queue();

		// set new capacity
		m_capacity = il.size();

		// allocate raw memory near this class.
		try
		{
			m_data = std::allocator_traits<Alloc>::allocate(m_alloc, m_capacity, this);
		}
		catch (...)
		{
			// transparent exceptions
			// leave container in valid state
			m_capacity = 0;
			m_head.ptr = m_tail.ptr = m_data = nullptr;
			m_head.parity = m_tail.parity = false;
			throw std::bad_alloc();
		}

		m_head.ptr = m_tail.ptr = m_data;
		m_head.parity = m_tail.parity = false;

		// fill class
		try
		{
			// fill container by copy constructing val
			for (auto itr = il.begin(); itr != il.end(); ++itr)
			{
				emplace_back(*itr);
			}
		}
		catch (...)
		{
			// if this fails, cleanup what's been done and then re-throw
			// if this fails, delete whats been made
			clear();
			throw;
		}
	}

	/**
     * @brief		Clear content
     * @details		Removes all elements from the deque (which are destroyed), leaving the container
     *				with a size of 0.\n
     *				<b> Complexity:</b> Linear in size (destructions).\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references related to
     *					this container are invalidated.\n
     *				<b> Data Races:</b> The container is modified.
     *					All contained elements are modified.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions.\n
     */
	void clear() noexcept
	{
		// destroy contained objects
		for (iterator itr = this->begin(); itr != this->end(); ++itr)
		{
			std::allocator_traits<Alloc>::destroy(m_alloc, const_cast<T*>(&itr));
		}
		// reset pointers. DON'T deallocate memory.
		m_head.ptr = m_tail.ptr = m_data;
		m_head.parity = m_tail.parity = false;
	}

	/**
     * @brief		Add element at the end
     * @details		Adds a new element at the end of the container, after its current last element.
     *				The content of val is copied (or moved) to the new element. This effectively
     *				increases the container size by one. If the circular_queue is full, it will cause
     *				the element at the front of the queue to be overwritten.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> Only the end iterator is invalidated, and all iterators,
     *					pointers and references to elements are guaranteed to keep referring to the
     *					same elements they were referring to before the call.\n
     *				<b> Data Races:</b> The container is modified. If full, the first element in the
     *					queue is modified. Concurrently accessing or modifying other elements is safe.\n
     *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
     *					(basic guarantee). If the element is copyable or no-throw movable, then there
     *					are no side effects (strong guarantee). If the container is not full, then there
     *					are no side effects (strong guarantee). If the container is full, and an
     *					exception occurs, the first element in the container will be destroyed. If
     *					allocator_traits::construct is not supported with val as argument, it causes
     *					undefined behavior.\n
     * @param[in]	val value to copy/move into the end of the container. Member type value_type is
     *					the type of the elements in the container, defined in vector as an alias of
     *					its first template parameter (T).
     */
	void push_back(const value_type& val)
	{
		// make sure if the push fails the queue is in a valid state
		try
		{
			if (full())
			{
				// copy assign the new object. Unlike emplace, no need to pop because we'll just
				// hijack the existing object instead.
				value_type* pos = m_head.ptr;
				m_head          = increment(m_head, 1);
				*pos            = val;    // this can throw
			}
			else
			{
				// copy construct the new object. This could throw.
				std::allocator_traits<Alloc>::construct(m_alloc, m_tail.ptr, val);    // this can throw
			}
		}
		catch (...)
		{
			// transparent exceptions
			throw std::bad_alloc();
		}

		// it allocated OK, update state
		m_tail = increment(m_tail, 1);
	}

	/// @overload
	void push_back(value_type&& val)
	{
		// make sure if the push fails the queue is in a valid state
		try
		{
			if (full())
			{
				// copy assign the new object. Unlike emplace, no need to pop because we'll just
				// hijack the existing object instead.
				value_type* pos = m_head.ptr;
				m_head          = increment(m_head, 1);
				*pos            = std::move(val);    // this can throw
			}
			else
			{
				// copy construct the new object. This could throw.
				std::allocator_traits<Alloc>::construct(m_alloc, m_tail.ptr, std::move(val));    // this can throw
			}
		}
		catch (...)
		{
			// transparent exceptions
			throw std::bad_alloc();
		}

		// it allocated OK, update state
		m_tail = increment(m_tail, 1);
	}

	/**
     * @brief		Add element at beginning
     * @details		Adds a new element at the beginning of the container, right before its current first element.
     *				The content of val is copied (or moved) to the new element. This effectively
     *				increases the container size by one. If the circular_queue is full, it will cause
     *				the element at the end of the queue to be overwritten.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> Only the begin iterator is invalidated, and all iterators,
     *					pointers and references to elements are guaranteed to keep referring to the
     *					same elements they were referring to before the call.\n
     *				<b> Data Races:</b> The container is modified. If full, the last element in the
     *					queue is modified. Concurrently accessing or modifying other elements is safe.\n
     *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
     *					(basic guarantee). If the element is copyable or no-throw movable, then there
     *					are no side effects (strong guarantee). If allocator_traits::construct is
     *					not supported with val as argument, it causes undefined behavior.\n
     * @param[in]	val value to copy/move into the end of the container. Member type value_type is
     *					the type of the elements in the container, defined in vector as an alias of
     *					its first template parameter (T).
     */
	void push_front(const value_type& val)
	{
		// make sure if the push fails the queue is in a valid state
		try
		{
			if (full())
			{
				// copy assign the new object. Unlike emplace, no need to pop because we'll just
				// hijack the existing object instead.
				m_head      = decrement(m_head, 1);
				*m_head.ptr = val;
				m_tail      = decrement(m_tail, 1);
			}
			else
			{
				// copy construct the new object. This could throw.
				m_head = decrement(m_head, 1);
				std::allocator_traits<Alloc>::construct(m_alloc, m_head.ptr, val);
			}
		}
		catch (...)
		{
			// return head to it's previous position
			m_head = increment(m_head, 1);

			// transparent exceptions
			throw std::bad_alloc();
		}
	}

	/// @overload
	void push_front(value_type&& val)
	{
		// make sure if the push fails the queue is in a valid state
		try
		{
			if (full())
			{
				// copy assign the new object. Unlike emplace, no need to pop because we'll just
				// hijack the existing object instead.
				m_head      = decrement(m_head, 1);
				*m_head.ptr = std::move(val);
				m_tail      = decrement(m_tail, 1);
			}
			else
			{
				// copy construct the new object. This could throw.
				m_head = decrement(m_head, 1);
				std::allocator_traits<Alloc>::construct(m_alloc, m_head.ptr, std::move(val));
			}
		}
		catch (...)
		{
			// return head to it's previous position
			m_head = increment(m_head, 1);

			// transparent exceptions
			throw std::bad_alloc();
		}
	}

	/**
     * @brief		Delete last element
     * @details		Removes the last element in the circular_queue container, effectively reducing
     *				its size by one. This destroys the removed element. Popping an empty queue
     *				results in undefined behavior.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> The iterators, pointers and references referring to
     *					the removed element are invalidated. Iterators, pointers and references
     *					referring to other elements that have not been removed are guaranteed to
     *					keep referring to the same elements they were referring to before the call.\n
     *				<b> Data Races:</b> The container is modified. The last element is modified.
     *					Concurrently accessing or modifying other elements is safe (although see
     *					iterator validity above).\n
     *				<b> Exception Safety:</b> If the container is not empty, the function never
     *					throws exceptions (no-throw guarantee). Otherwise, it causes undefined behavior.
     */
	inline void pop_back() noexcept
	{
		m_tail = decrement(m_tail, 1);
		std::allocator_traits<Alloc>::destroy(m_alloc, m_tail.ptr);
	}

	/**
     * @brief		Delete first element
     * @details		Removes the first element in the circular_queue container, effectively reducing
     *				its size by one. This destroys the removed element. Popping an empty queue
     *				results in undefined behavior.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> The iterators, pointers and references referring to
     *					the removed element are invalidated. Iterators, pointers and references
     *					referring to other elements that have not been removed are guaranteed to
     *					keep referring to the same elements they were referring to before the call.\n
     *				<b> Data Races:</b> The container is modified. The first element is modified.
     *					Concurrently accessing or modifying other elements is safe (although see
     *					iterator validity above).\n
     *				<b> Exception Safety:</b> If the container is not empty, the function never
     *					throws exceptions (no-throw guarantee). Otherwise, it causes undefined behavior.
     */
	inline void pop_front() noexcept
	{
		std::allocator_traits<Alloc>::destroy(m_alloc, m_head.ptr);    // if your destructor throws than you are an asshole (and not writing valid c++) and don't you dare blame my container.
		m_head = increment(m_head, 1);
	}

	/**
     * @brief		Insert (copy) single element
     * @details		The container is extended by inserting an element before the element at the
     *				specified position. This effectively increases the container size by 1. If the
     *				container is full, the element at begin() is overwritten. Makes at most 1 copy
     *				of val.\n
     *				Double-ended queues are designed to be efficient performing insertions (and removals)
     *				from either the end or the beginning of the sequence. Insertions on other positions
     *				are usually less efficient than in list or forward_list containers.\n
     *				<b> Complexity:</b> Linear on the number of elements inserted (copy/move
     *					construction) plus an additional linear in the number of elements between
     *					position and the end of the container.\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references from position
     *					to the end of the queue are invalidated.\n
     *				<b> Data Races:</b> The container is modified. It is not safe to concurrently
     *					access elements.\n
     *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
     *					(basic guarantee). If allocator_traits::construct is not supported with the
     *					appropriate arguments for the element constructions, or if an invalid position
     *					or range is specified, it causes undefined behavior.\n
     * @param[in]	position Position in the container where the new elements are inserted. Iterator
     *				is a member type, defined as a random access iterator type that points to elements.
     * @param[in]	val Value to be copied (or moved) to the inserted elements. Member type value_type
     *				is the type of the elements in the container, defined in deque as an alias of
     *				its first template parameter (T).
     * @sa			push_back()
     * @sa			push_front()
     * @returns		An iterator that points to the newly inserted element.
     */
	iterator insert(const_iterator position, const value_type& val)
	{
		assert(position.m_buffer == this);

		// increase size of container by 1
		push_back(val);

		// swap the new element down to the position we're supposed to insert it at
		return swapDownTo(position, 1);
	}

	/**
    * @brief		Insert (move) single element
    * @details		The container is extended by inserting an element before the element at the
    *				specified position. This effectively increases the container size by 1. If the
    *				container is full, the element at begin() is overwritten. Makes no copies of val,
    *				and val must be <i>move-assignable</i>.\n
    *				Double-ended queues are designed to be efficient performing insertions (and removals)
    *				from either the end or the beginning of the sequence. Insertions on other positions
    *				are usually less efficient than in list or forward_list containers.\n
    *				<b> Complexity:</b> Linear on the number of elements inserted (copy/move
    *					construction) plus an additional linear in the number of elements between
    *					position and the end of the container.\n
    *				<b> Iterator Validity:</b> All iterators, pointers and references from position
    *					to the end of the queue are invalidated.\n
    *				<b> Data Races:</b> The container is modified. It is not safe to concurrently
    *					access elements.\n
    *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
    *					(basic guarantee). If allocator_traits::construct is not supported with the
    *					appropriate arguments for the element constructions, or if an invalid position
    *					or range is specified, it causes undefined behavior.\n
    * @param[in]	position Position in the container where the new elements are inserted. Iterator
    *				is a member type, defined as a random access iterator type that points to elements.
    * @param[in]	val Value to be copied (or moved) to the inserted elements. Member type value_type
    *				is the type of the elements in the container, defined in deque as an alias of
    *				its first template parameter (T).
    * @sa			push_back()
    * @sa			push_front()
    * @returns		An iterator that points to the newly inserted element.
    */
	iterator insert(const_iterator position, value_type&& val)
	{
		assert(position.m_buffer == this);

		// increase size of container by 1
		push_back(std::move(val));

		// swap the new element down to the position we're supposed to insert it at
		return swapDownTo(position, 1);
	}

	/**
     * @brief		Insert (fill) elements
     * @details		The container is extended by inserting new elements before the element at the
     *				specified position. This effectively increases the container size by the amount
     *				of elements inserted. If this size is greater than the current capacity,
     *				the elements at the beginning of the queue will be over-written. This operation
     *				will cause at most n copies of <i>val</i>.\n
     *				Double-ended queues are designed to be efficient performing insertions (and removals)
     *				from either the end or the beginning of the sequence. Insertions on other
     *				positions are usually less efficient than in list or forward_list containers.\n
     *				<b> Complexity:</b> O(m*n) where m is the current size and m is the number of
     *					elements inserted.\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references are invalidated.\n
     *				<b> Data Races:</b> The container is modified. It is not safe to concurrently
     *					access elements.\n
     *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
     *					(basic guarantee). If allocator_traits::construct is not supported with the
     *					appropriate arguments for the element constructions, or if an invalid position
     *					or range is specified, it causes undefined behavior.\n
     * @param[in]	position the fill will be inserted before the element at this position.
     * @param[in]	n Number of elements to insert. Each element is initialized to a copy of val.
     * @param[in]	val Value to be copied to the inserted elements. Member type value_type is the
     *				type of the elements in the container, defined in circular_queue as an alias of
     *				its first template parameter (T).
     * @sa			push_back()
     * @sa			push_front()
     * @returns		An iterator that points to the first of the newly inserted elements.
     */
	iterator insert(const_iterator position, size_type n, const value_type& val)
	{
		assert(position.m_buffer == this);

		// increase size of container by n
		for (size_type i = 0; i < (n < capacity() ? n : capacity()); ++i)
		{
			push_back(val);    // this can change begin()
		}

		// if we filled more than the capacity, no need to swap
		if (n >= capacity()) return begin();

		// swap the new elements down to the position we're supposed to insert it at
		return swapDownTo(position, n);
	}

	/**
     * @brief		Insert (range) elements
     * @details		The container is extended by inserting new elements before the element at the
     *				specified position. This effectively increases the container size by the amount
     *				of elements inserted. If this size is greater than the current capacity,
     *				the elements at the beginning of the queue will be over-written. This operation
     *				will cause at most capacity() copies. The range of copied values is [first, last).\n
     *				Double-ended queues are designed to be efficient performing insertions (and removals)
     *				from either the end or the beginning of the sequence. Insertions on other
     *				positions are usually less efficient than in list or forward_list containers.\n
     *				<b> Complexity:</b> Linear on the number of elements inserted (copy/move
     *					construction) plus an additional linear in the number of elements between
     *					position and the end of the container.\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references are invalidated.\n
     *				<b> Data Races:</b> The container is modified. It is not safe to concurrently
     *					access elements.\n
     *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
     *					(basic guarantee). If allocator_traits::construct is not supported with the
     *					appropriate arguments for the element constructions, or if an invalid position
     *					or range is specified, it causes undefined behavior.\n
     * @param[in]	position the fill will be inserted before the element at this position.
     * @param[in]	first iterator to beginning of range to insert. InputIterator can be an iterator
     *				to any type of container, as long as it is at least a forward iterator.
     * @param[in]	last iterator to one-past-end of range to insert. InputIterator can be an iterator
     *				to any type of container, as long as it is at least a forward iterator.
     *				type of the elements in the container, defined in circular_queue as an alias of
     *				its first template parameter (T).
     * @sa			push_back()
     * @sa			push_front()
     * @returns		An iterator that points to the first of the newly inserted elements.
     */
	template<class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last)
	{
		static_assert(std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<InputIterator>::iterator_category>::value,
		              "Insert (range) requires InputIterator to be at least a forward iterator.");

		static_assert(std::is_convertible<typename std::iterator_traits<InputIterator>::value_type, value_type>::value,
		              "InputIterator::value_type is not implicitly convertible to circular_queue::value_type.");

		assert(position.m_buffer == this);

		size_type n = last - first;

		InputIterator start = first;
		if (n > capacity())
		{
			start = last - capacity();
		}

		// increase size of container by n
		for (InputIterator i = std::move(start); i < last; ++i)
		{
			push_back(*i);    // this can change begin()
		}

		// if we filled more than the capacity, no need to swap
		if (n >= capacity()) return begin();

		// swap the new elements down to the position we're supposed to insert it at
		return swapDownTo(position, n);
	}

	/**
     * @brief		Insert elements (initializer_list)
     * @details		The container is extended by inserting new elements before the element at the
     *				specified position. This effectively increases the container size by the amount
     *				of elements inserted. If this size is greater than the current capacity,
     *				the elements at the beginning of the queue will be over-written. This operation
     *				will cause at most capacity() copies.\n
     *				Double-ended queues are designed to be efficient performing insertions (and removals)
     *				from either the end or the beginning of the sequence. Insertions on other
     *				positions are usually less efficient than in list or forward_list containers.\n
     *				<b> Complexity:</b> Linear on the number of elements inserted (copy/move
     *					construction) plus an additional linear in the number of elements between
     *					position and the end of the container.\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references are invalidated.\n
     *				<b> Data Races:</b> The container is modified. It is not safe to concurrently
     *					access elements.\n
     *				<b> Exception Safety:</b> The container is guaranteed to end in a valid state
     *					(basic guarantee). If allocator_traits::construct is not supported with the
     *					appropriate arguments for the element constructions, or if an invalid position
     *					or range is specified, it causes undefined behavior.\n
     * @param[in]	position the fill will be inserted before the element at this position.
     * @param[in]	il initializer_list object, filled with values to insert into the container.
     * @sa			push_back()
     * @sa			push_front()
     * @returns		An iterator that points to the first of the newly inserted elements.
     */
	iterator insert(const_iterator position, std::initializer_list<value_type> il)
	{
		return insert(position, il.begin(), il.end());
	}

	/**
     * @brief		Erase element
     * @details		Removes from deque container a single element (<i>position</i>).
     *				This effectively reduces the container size by the number of elements removed,
     *				which are destroyed.\n
     *				Double-ended queues are designed to be efficient removing (and inserting) elements
     *				at either the end or the beginning of the sequence. Removals on other positions
     *				are usually less efficient than in list or forward_list containers.
     *				<b> Complexity:</b> Linear in the distance from <i>position</i> to end().\n
     *				<b> Iterator Validity:</b> All iterators, pointers, and references to elements
     *					between <i>position</i> and end() are invalidated.\n
     *				<b> Data Races:</b> If the erasure happens at the end of the sequence, only the
     *					erased elements are modified. Otherwise it is not safe to access or modify
     *					elements.\n
     *				<b> Exception Safety:</b> If the removed elements include the the last element
     *					in the container, no exceptions are thrown (no-throw guarantee). Otherwise,
     *					the container is guaranteed to end in a valid state (basic guarantee): Copying
     *					or moving elements while relocating them may throw. Invalid ranges produce
     *					undefined behavior.
     * @param[in]	position Iterator pointing to a single element to be removed from the container.
     * @returns		An iterator pointing to the new location of the element that followed the last
     *				element erased by the function call. This is the container end if the operation
     *				erased the last element in the sequence.
     */
	iterator erase(const_iterator position)
	{
		assert(position.m_buffer == this);

		// swap the elements to delete to the back of the queue
		swapUpToEnd(position, 1);
		pop_back();

		return iterator(this, position.m_pointer);
	}

	/**
     * @brief		Erase elements
     * @details		Removes from the deque container the range of elements ([<i>first</i>,<i>last</i>)).
     *				This effectively reduces the container size by the number of elements removed,
     *				which are destroyed.\n
     *				Double-ended queues are designed to be efficient removing (and inserting) elements
     *				at either the end or the beginning of the sequence. Removals on other positions
     *				are usually less efficient than in list or forward_list containers.
     *				<b> Complexity:</b> Linear in the distance from <i>position</i> to end() times
     *					the number of elements erased.\n
     *				<b> Iterator Validity:</b> All iterators, pointers, and references to elements
     *					between <i>position</i> and end() are invalidated.\n
     *				<b> Data Races:</b> If the erasure happens at the end of the sequence, only the
     *					erased elements are modified. Otherwise it is not safe to access or modify
     *					elements.\n
     *				<b> Exception Safety:</b> If the removed elements include the the last element
     *					in the container, no exceptions are thrown (no-throw guarantee). Otherwise,
     *					the container is guaranteed to end in a valid state (basic guarantee): Copying
     *					or moving elements while relocating them may throw. Invalid ranges produce
     *					undefined behavior.
     * @param[in]	first iterator to beginning of range to erase from the container
     * @param[in]	last iterator to one-past-last element of range to erase from the container
     * @returns		An iterator pointing to the new location of the element that followed the last
     *				element erased by the function call. This is the container end if the operation
     *				erased the last element in the sequence.
     */
	iterator erase(const_iterator first, const_iterator last)
	{
		assert(first.m_buffer == this);
		assert(last.m_buffer == this);
		assert(last > first);

		difference_type n = last - first;

		swapUpToEnd(first, n);

		for (difference_type i = 0; i < n; ++i)
		{
			pop_back();
		}

		return iterator(this, first.m_pointer);
	}

	/**
     * @brief		Swap content
     * @details		Exchanges the content of the container by the content of <i>other</i>, which is
     *				another circular_queue object containing elements of the same type. Sizes may differ.
     *				After the call to this member function, the elements in this container are those
     *				which were in <i>other</i> before the call, and the elements of <i>other</i> are
     *				those which were in <i>this</i>. All iterators, references and pointers remain
     *				valid for the swapped objects. Notice that a non-member function exists with the
     *				same name, swap, overloading that algorithm with an optimization that behaves
     *				like this member function.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> All iterators, pointers and references referring to
     *				elements in both containers remain valid, and are now referring to the same elements
     *				they referred to before the call, but in the other container, where they now iterate.\n
     *				<b> Data Races:</b> Both the container and <i>other</i> are modified. No contained
     *				elements are accessed by the call.\n
     *				<b> Exception Safety:</b> If the allocators in both containers compare equal, or
     *				if their allocator traits indicate that the allocators shall propagate, the
     *				function never throws exceptions (no-throw guarantee). Otherwise, it causes
     *				undefined behavior.\n
     * @param[in]	other container to swap with <i>this</i>.
     */
	void swap(circular_queue& other) noexcept
	{
		if (std::allocator_traits<decltype(other.m_alloc)>::propagate_on_container_swap::value)
		{
			allocator_type temp = this->m_alloc;
			this->m_alloc       = other.m_alloc;
			other.m_alloc       = temp;
		}

		auto tempData = this->m_data;
		this->m_data  = other.m_data;
		other.m_data  = tempData;

		auto tempCapacity = this->m_capacity;
		this->m_capacity  = other.m_capacity;
		other.m_capacity  = tempCapacity;

		auto tempHead = this->m_head;
		this->m_head  = other.m_head;
		other.m_head  = tempHead;

		auto tempTail = this->m_tail;
		this->m_tail  = other.m_tail;
		other.m_tail  = tempTail;
	}

	/**
     * @brief		Construct and insert element
     * @details		The container is extended by inserting a new element at position. This new element
     *				is constructed in place using args as the arguments for its construction. This
     *				effectively increases the container size by one. Double-ended queues are designed
     *				to be efficient performing insertions (and removals) from either the end or the
     *				beginning of the sequence. Insertions on other positions are usually less efficient
     *				than in list or forward_list containers. See emplace_front and emplace_back for
     *				member functions that extend the container directly at the beginning or at the end.
     *				The element is constructed in-place by calling allocator_traits::construct with
     *				args forwarded.\n
     *				<b> Complexity:</b> Linear in the number of elements between <i>position</i> and end().\n
     *				<b> Iterator Validity:</b> All iterators, references, and pointers from
     *					<i>position</i> to end() are invalidated.\n
     *				<b> Data Races:</b> The container is modified. It is NOT safe to concurrently
     *					access or modify elements.\n
     *				<b> Exception Safety:</b> If position is end, there are no changes in the
     *					container in case of exception (strong guarantee). Otherwise, the container
     *					is guaranteed to end in a valid state (basic guarantee).\n
     * @param[in]	position Position in the container where the new element is inserted. Member type
     *				const_iterator is a random access iterator type that points to a constant element.
     * @param[in]	args Arguments forwarded to construct the new element.
     * @sa			emplace_front()
     * @sa			emplace_back()
     */
	template<class... Args>
	iterator emplace(const_iterator position, Args&&... args)
	{
		assert(position.m_buffer == this);

		// increase size of container by 1
		emplace_back(std::forward<Args>(args)...);

		// swap the new element down to the position we're supposed to insert it at
		return swapDownTo(position, 1);
	}

	/**
     * @brief		Construct and insert element at beginning
     * @details		Inserts a new element at the beginning of the container, right before its
     *				current first element. This new element is constructed in place using args as
     *				the arguments for its construction.This effectively increases the container size by one.
     *				The element is constructed in-place by calling allocator_traits::construct with
     *				args forwarded. A similar member function exists, push_front, which either
     *				copies or moves an existing object into the container.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> The begin() iterator is invalidated. If the container was
     *					full before the call, the end iterator is also invalidated. All other
     *					iterators, pointers, and references remain valid.\n
     *				<b> Data Races:</b> The container is modified. If the container was full, the
     *					last element is destroyed. No other existing elements are accessed.\n
     *				<b> Exception Safety:</b> If the container is not full, there are no side effects
     *					(strong guarantee). Otherwise, if the function throws an exception, no
     *					resources are leaked (basic guarantee), however the head of the container will be destroyed
     *					even if emplace_back throws and no new object is added to the container.\n
     * @param[in]	args arguments to the constructor of the contained object.
     */
	template<class... Args>
	void emplace_front(Args&&... args)
	{
		// make sure if the construction fails the queue is in a valid state
		try
		{
			// cleanup what the tail points to before over-writing it if we are full
			if (full())
			{
				pop_back();
			}

			m_head = decrement(m_head, 1);

			// construct the new object. This could throw.
			std::allocator_traits<Alloc>::construct(m_alloc, m_head.ptr, std::forward<Args>(args)...);
		}
		catch (...)
		{
			// undo head decrement
			m_head = increment(m_head, 1);

			// transparent exceptions
			throw std::bad_alloc();
		}
	}

	/**
     * @brief		Construct and insert element at the end
     * @details		Inserts a new element at the end of the circular_queue, right after its current
     *				last element. This new element is constructed in place using args as the
     *				arguments for its construction. This effectively increases the container capacity by
     *				one. The element is constructed in-place by calling allocator_traits::construct
     *				with args forwarded. A similar member function exists, push_back, which either
     *				copies or moves an existing object into the container.\n
     *				<b> Complexity:</b> Constant, if the contained object can be constructed in
     *					constant time.\n
     *				<b> Iterator Validity:</b> The end iterator is invalidated. If the container was
     *					full before the call, the begin iterator is also invalidated. All other
     *					iterators, pointers, and references remain valid.\n
     *				<b> Data Races:</b> The container is modified. If the container was full, the
     *					beginning element is destroyed. No other existing elements are accessed.\n
     *				<b> Exception Safety:</b> If the container is not full, there are no side effects
     *					(strong guarantee). Otherwise, if the function throws an exception, no
     *					resources are leaked (basic guarantee), however the head of the container will be destroyed
     *					even if emplace_back throws and no new object is added to the container.\n
     * @param[in]	args arguments to the constructor of the contained object.
     */
	template<class... Args>
	void emplace_back(Args&&... args)
	{
		// make sure if the construction fails the queue is in a valid state
		try
		{
			// cleanup what the tail points to before over-writing it if we are full
			if (full())
			{
				pop_front();
			}

			// construct the new object. This could throw.
			std::allocator_traits<Alloc>::construct(m_alloc, m_tail.ptr, std::forward<Args>(args)...);

			// it allocated OK, update state
			m_tail = increment(m_tail, 1);
		}
		catch (...)
		{
			// transparent exceptions
			throw std::bad_alloc();
		}
	}

	/** @} */    // end of modifiers

	//////////////////////////////////////////////////////////////////////////
	//		#ACCESSORS
	//////////////////////////////////////////////////////////////////////////

	/**
     * @{
     * @name	Accessors
     * @brief	Access member objects of the container
     */

	/**
     * @brief		Get allocator
     * @details		Returns a copy of the allocator object associated with the container.\n
     *				<b> Complexity:</b> Constant.\n
     *				<b> Iterator Validity:</b> No changes.\n
     *				<b> Data Races:</b> The container is accessed. No contained elements are
     *					accessed: concurrently accessing or modifying them is safe.\n
     *				<b> Exception Safety:</b> No-throw guarantee: this member function never throws
     *					exceptions. Copying any instantiation of the default allocator is also
     *					guaranteed to never throw.\n
     * @returns		the allocator.
     */
	allocator_type get_allocator() const noexcept
	{
		return m_alloc;
	}

	/**
     * @brief		Copies range to destination buffer using provided copy function.
     * @details		This function overloads memcpy to handle the internal complexities of copying a
     *				circular_queue, which may or may not be wrapped. The resulting array at destination
     *				will be unwrapped in either case, meaning logical and physical index[0] will always
     *				be the same. Since this function isn't allocator or iterator aware on the destination
     *				it's use should be avoided for EVERYTHING except compatibility with C code. In the
     *				author's opinion, the ONLY use case for this function is CUDA memory.\n
     *				*THIS MAY NOT WORK* if <i>value_type</i> does not exhibit bitwise copy semantics.
     * @param[in]	first iterator to the first index to be copied.
     * @param[in]	last iterator to the last index to be copied.
     * @param[out]	destination output buffer where the data will be copied to.
     * @param[in]	copy functor used to perform the memory copy. Options may include memcpy, memmove,
     *				or bound versions of cudaMemcpy, cudaMemcpyAsync. Must have a signature of
     *				copyFunctor(void* destination, void* source, size_t sizeInBytesToCopy)
     * @returns		forwards the return value of <i>copyFunctor</i>.
     */
	template<class copyFunctor>
	auto copy(const_iterator first, const_iterator last, value_type* destination, copyFunctor cf) const -> decltype(cf((void*) destination, (void*) first.m_pointer.ptr, ((last.m_pointer.ptr - first.m_pointer.ptr) * sizeof(value_type))))
	{

		if (first.m_pointer.parity == last.m_pointer.parity)
		{
			// straight copy when the iterators have the same parity
			return cf((void*) destination, (void*) first.m_pointer.ptr, ((last.m_pointer.ptr - first.m_pointer.ptr) * sizeof(value_type)));
		}
		else
		{
			// if the buffer is wrapped, we need to mem copy in two parts
			size_t firstChunkSize = m_data + m_capacity - first.m_pointer.ptr;    // units of value_type, NOT bytes

			cf((void*) destination, (void*) first.m_pointer.ptr, firstChunkSize * sizeof(value_type));
			return cf(destination + firstChunkSize, (void*) m_data, (last.m_pointer.ptr - m_data) * sizeof(value_type));
		}
	}

	/** @} */    // end of accessors

protected:
	//////////////////////////////////////////////////////////////////////////
	//		PROTECTED MEMBER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	/**
     * @brief		increments a pointer
     * @details		Takes care of wrapping the pointer and keeping parity consistent.
     * @param[in]	p pointer to increment. This function modifies the value of p.
     * @param[in]	n number of elements to increment the pointer
     */
	inline queue_pointer increment(queue_pointer p, difference_type n) const noexcept
	{
		p.ptr += n;

		// wrap tail if it hits end of the buffer
		if (p.ptr >= this->m_data + this->m_capacity)
		{
			p.ptr = m_data + (p.ptr - (m_data + m_capacity));
			// flip the parity
			p.parity = !p.parity;
			return p;
		}
		else if (p.ptr < m_data)
		{
			// annoyingly, 'n' can be negative, so this check is required.
			p.ptr = m_data + m_capacity - (m_data - p.ptr);
			// flip parity
			p.parity = !p.parity;
			return p;
		}

		return p;
	}

	/// @overload
	inline const_queue_pointer increment(const_queue_pointer p, difference_type n) const noexcept
	{
		p.ptr += n;

		// wrap tail if it hits end of the buffer
		if (p.ptr >= m_data + m_capacity)
		{
			p.ptr = m_data + (p.ptr - (m_data + m_capacity));
			// flip the parity
			p.parity = !p.parity;
			return p;
		}
		else if (p.ptr < m_data)
		{
			// annoyingly, 'n' can be negative, so this check is required.
			p.ptr = m_data + m_capacity - (m_data - p.ptr);
			// flip parity
			p.parity = !p.parity;
			return p;
		}

		return p;
	}

	/**
    * @brief		decrements a pointer
    * @details		Takes care of wrapping the pointer and keeping parity consistent.
    * @param[in]	p pointer to decrement. This function modifies the value of p.
    * @param[in]	n number of elements to increment the pointer
    * @param[in]	parity parity bit associated with the pointer. This function modifies the value
    *				of parity.
    */
	inline queue_pointer decrement(queue_pointer p, difference_type n) const noexcept
	{
		p.ptr -= n;

		// wrap the tail if we hit the beginning of the buffer
		if (p.ptr < m_data)
		{
			p.ptr = m_data + m_capacity - (m_data - p.ptr);
			// flip parity
			p.parity = !p.parity;
			return p;
		}
		if (p.ptr >= m_data + m_capacity)
		{
			// annoyingly, 'n' can be negative, so this check is required.
			p.ptr = m_data + (p.ptr - (m_data + m_capacity));
			// flip the parity
			p.parity = !p.parity;
			return p;
		}

		return p;
	}

	/// @overload
	inline const_queue_pointer decrement(const_queue_pointer p, difference_type n, bool& parity) const noexcept
	{
		p.ptr -= n;

		// wrap the tail if we hit the beginning of the buffer
		if (p.ptr < m_data)
		{
			p.ptr = m_data + m_capacity - (m_data - p.ptr);
			// flip parity
			p.parity = !p.parity;
			return p;
		}
		if (p.ptr >= m_data + m_capacity)
		{
			// annoyingly, 'n' can be negative, so this check is required.
			p.ptr = m_data + (p.ptr - (m_data + m_capacity));
			// flip the parity
			p.parity = !p.parity;
			return p;
		}

		return p;
	}

	/**
     * @brief		moves the currently contained data to a new area with size n.
     * @details		O(n) complexity. Unwraps the buffer as a side effect.
     * @param[in]	n size of new allocation
     */
	void reallocate(size_type n)
	{
		pointer newData;
		pointer newDataItr;

		try
		{
			newData = newDataItr = std::allocator_traits<Alloc>::allocate(m_alloc, n, this);
		}
		catch (...)
		{
			throw std::bad_alloc();
		}

		// move construct the queue to the new location. We can't just memcpy because we don't want
		// the queue to be wrapped with a new maximum size, it won't work.
		try
		{
			for (auto&& front : *this)
			{
				std::allocator_traits<Alloc>::construct(m_alloc, newDataItr, std::move(front));
				pop_front();    // this provides some exception safety by making sure the container state is always valid, even though we're going to overwrite it in a few lines.
				newDataItr++;
			}
		}
		catch (...)
		{
			// increment head past the thing that blew up
			m_head = increment(m_head, 1);

			// free the new data area
			pointer newDataDestroyer = newData;
			while (newDataDestroyer < newDataItr)
			{
				std::allocator_traits<Alloc>::destroy(m_alloc, newDataDestroyer);
				++newDataDestroyer;
			}
			m_alloc.deallocate(newData, n);

			// just re-throw whatever happened.
			throw;
		}

		// deallocate old resources
		m_alloc.deallocate(m_data, m_capacity);

		// everything is moved, reset the internal state
		m_data = m_head.ptr = m_tail.ptr = newData;
		m_head.parity = m_tail.parity = false;
		m_capacity                    = n;

		// advance the tail
		difference_type distance = newDataItr - newData;
		for (difference_type diff = 0; diff < distance; ++diff)
		{
			m_tail = increment(m_tail, 1);
		}
	}

	/**
     * @brief		Swaps the back of the queue down to position with elements n positions away
     * @details		No-throw. O(n) complexity.
     * @param[in]	position position to swap down to from the end
     * @param[in]	n number of elements
     * @returns		iterator to position of last element swapped.
     */
	inline iterator swapDownTo(const_iterator position, size_type n) noexcept
	{
		// create a non-const position iterator
		iterator pos(this, const_cast<value_type*>(position.m_pointer.ptr), position.m_pointer.parity);

		// the end iterator is the max of pos or begin (in case pos was consumed by push_back)
		pos = pos > begin() ? pos : begin();

		if (pos == end() - n) return pos;

		// swap val all the way down to its resting position
		for (size_type i = 0; i < n; ++i)
		{
			for (iterator swapItr = --end(); swapItr != pos; --swapItr)
			{
				// swap
				auto temp      = std::move(*swapItr);
				*swapItr       = std::move(*(swapItr - 1));
				*(swapItr - 1) = std::move(temp);
			}
		}

		return pos;
	}

	/**
     * @brief		Moves the given position to the end of the queue by successively swapping it.
     * @details		No-throw. O(n) complexity.
     * @param[in]	position position to swap up to the end from
     * @param[in]	n number of elements
     * @returns		iterator to position of first element swapped.
     */
	inline iterator swapUpToEnd(const_iterator position, size_type n) noexcept
	{
		// create a non-const position iterator
		iterator pos(this, const_cast<pointer>(position.m_pointer.ptr), position.m_pointer.parity);

		// the end iterator is the max of pos or begin (in case pos was consumed by push_back)
		pos = pos > begin() ? pos : begin();

		if (pos == end() - n) return pos;

		// swap val all the way down to its resting position
		for (size_type i = 0; i < n; ++i)
		{
			for (iterator swapItr = pos; swapItr != end() - 1; ++swapItr)
			{
				// swap
				auto temp      = std::move(*swapItr);
				*swapItr       = std::move(*(swapItr + 1));
				*(swapItr + 1) = std::move(temp);
			}
		}

		return pos;
	}

protected:
	Alloc         m_alloc;       ///< Allocator class. NOT c-style malloc.
	pointer       m_data;        ///< Pointer to the internal data array.
	size_type     m_capacity;    ///< capacity of the buffer array.
	queue_pointer m_head;        ///< Points to the logical beginning of the buffer. No memory ownership!
	queue_pointer m_tail;        ///< Points to the logical end of the buffer. No memory ownership!
};

#endif    // circular_queue_h__