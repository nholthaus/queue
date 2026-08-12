#pragma once
#include <concurrent_queue.h>

#include <algorithm>
#include <barrier>
#include <future>
#include <list>
#include <memory>
#include <memory_resource>
#include <numeric>
#include <stdexcept>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::string_literals;

namespace
{
	template<class T>
	class stateful_test_allocator
	{
	public:
		using value_type = T;

		explicit stateful_test_allocator(int identity = 0) noexcept
		    : identity(identity)
		{
		}

		template<class U>
		stateful_test_allocator(const stateful_test_allocator<U>& other) noexcept
		    : identity(other.identity)
		{
		}

		[[nodiscard]] T* allocate(std::size_t n)
		{
			return std::allocator<T>{}.allocate(n);
		}

		void deallocate(T* ptr, std::size_t n) noexcept
		{
			std::allocator<T>{}.deallocate(ptr, n);
		}

		template<class U>
		friend class stateful_test_allocator;

		friend bool operator==(const stateful_test_allocator&, const stateful_test_allocator&) = default;

		int identity;
	};

	template<class T, class Alloc>
	class throwing_move_queue : public std::deque<T, Alloc>
	{
		using base_type = std::deque<T, Alloc>;

	public:
		using base_type::base_type;
		throwing_move_queue()                                      = default;
		throwing_move_queue(const throwing_move_queue&)            = default;
		throwing_move_queue& operator=(const throwing_move_queue&) = default;

		throwing_move_queue(throwing_move_queue&& other) noexcept(false)
		    : base_type(move_or_throw(other))
		{
		}

		throwing_move_queue(throwing_move_queue&& other, const Alloc& alloc) noexcept(false)
		    : base_type(move_or_throw(other), alloc)
		{
		}

		throwing_move_queue& operator=(throwing_move_queue&& other) noexcept(false)
		{
			throw_if_requested();
			base_type::operator=(std::move(other));
			return *this;
		}

		void swap(throwing_move_queue& other) noexcept(false)
		{
			throw_if_requested();
			base_type::swap(other);
		}

		inline static bool throw_on_move = false;

	private:
		static throwing_move_queue&& move_or_throw(throwing_move_queue& other)
		{
			throw_if_requested();
			return std::move(other);
		}

		static void throw_if_requested()
		{
			if (throw_on_move)
				throw std::runtime_error("requested queue operation failure");
		}
	};

	// The fixture for testing class Cache
	class concurrent_queue_test : public ::testing::Test
	{
	};

	// generator functor for odd numbers
	class generate_odd_numbers
	{
	public:
		int operator()()
		{
			return (++i * 2 + 1);
		}

	private:
		int i = -1;
	};

	// generator functor for even numbers
	class generate_even_numbers
	{
	public:
		int operator()()
		{
			return (++i * 2);
		}

	private:
		int i = 0;
	};

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, traits)
	{
		EXPECT_TRUE(std::is_constructible_v<concurrent_queue<int>>);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, default_constructor)
	{
		EXPECT_TRUE(std::is_default_constructible_v<concurrent_queue<int>>);

		test_object::clearfunctioncalls();
		{
			concurrent_queue<test_object> a;

			EXPECT_TRUE(a.empty());
			EXPECT_EQ(a.size(), 0);
			EXPECT_TRUE((std::is_same_v<std::allocator<test_object>, decltype(a.get_allocator())>) );
		}
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, default_constructor_with_allocator)
	{
		test_object::clearfunctioncalls();
		{
			using Allocator = std::pmr::polymorphic_allocator<test_object>;

			concurrent_queue<test_object, std::deque, Allocator> a(Allocator{});

			EXPECT_TRUE(a.empty());
			EXPECT_EQ(a.size(), 0);
			EXPECT_TRUE((std::is_same_v<Allocator, decltype(a.get_allocator())>) );
			EXPECT_FALSE((std::is_same_v<std::allocator<int>, decltype(a.get_allocator())>) );
		}
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, fill_constructor)
	{
		test_object::clearfunctioncalls();
		{
			concurrent_queue<test_object> a(3);
			EXPECT_EQ(a.size(), 3);
		}
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(3, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, fill_constructor_with_value)
	{
		test_object::clearfunctioncalls();
		{
			concurrent_queue<test_object> a(5, "a");
			EXPECT_EQ(a.size(), 5);
			for (auto itr = a.cbegin(); itr < a.cend(); ++itr)
				EXPECT_STREQ(itr->getString().data(), "a");
		}
		EXPECT_EQ(1, test_object::getconstructorcalls());
		EXPECT_EQ(6, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(5, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, fill_constructor_with_allocator)
	{
		test_object::clearfunctioncalls();
		{
			concurrent_queue a(5, test_object("b"), std::pmr::polymorphic_allocator<test_object>{});
			EXPECT_EQ(a.size(), 5);
			for (auto itr = a.cbegin(); itr < a.cend(); ++itr)
				EXPECT_EQ(*itr, "b");
			EXPECT_TRUE((std::is_same_v<std::pmr::polymorphic_allocator<test_object>, decltype(a.get_allocator())>) );
			EXPECT_FALSE((std::is_same_v<std::allocator<test_object>, decltype(a.get_allocator())>) );
		}
		EXPECT_EQ(1, test_object::getconstructorcalls());
		EXPECT_EQ(6, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(5, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, copy_constructor)
	{
		EXPECT_TRUE(std::is_copy_constructible_v<concurrent_queue<int>>);

		test_object::clearfunctioncalls();
		{
			test_object      c('c');
			concurrent_queue a(3, c);
			auto             b(a);

			EXPECT_EQ(a, b);
			EXPECT_EQ(b.size(), 3);

			for (auto itr = b.cbegin(); itr < b.cend(); ++itr)
			{
				EXPECT_EQ(*itr, c);
			}
		}
		EXPECT_EQ(1, test_object::getconstructorcalls());
		EXPECT_EQ(7, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(6, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, copy_constructor_with_allocator)
	{
		test_object::clearfunctioncalls();
		{
			test_object      c('c');
			concurrent_queue a(3, c, std::pmr::polymorphic_allocator<test_object>{});
			auto             b(a);

			EXPECT_EQ(a, b);
			EXPECT_EQ(b.size(), 3);

			for (auto itr = b.cbegin(); itr < b.cend(); ++itr)
			{
				EXPECT_EQ(*itr, c);
			}

			EXPECT_TRUE((std::is_same_v<std::pmr::polymorphic_allocator<test_object>, decltype(a.get_allocator())>) );
			EXPECT_FALSE((std::is_same_v<std::allocator<test_object>, decltype(a.get_allocator())>) );
		}
		EXPECT_EQ(1, test_object::getconstructorcalls());
		EXPECT_EQ(7, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(6, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, move_constructor)
	{
		EXPECT_TRUE(std::is_move_constructible_v<concurrent_queue<test_object>>);

		concurrent_queue<test_object> a{'a', 'b', 'c'};

		test_object::clearfunctioncalls();
		concurrent_queue<test_object> b(std::move(a));
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);

		EXPECT_FALSE(b.empty());
		EXPECT_EQ(b.size(), 3);

		test_object tmp;
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "a");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "b");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "c");
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, move_constructor_with_allocator)
	{
		EXPECT_TRUE(std::is_move_constructible_v<concurrent_queue<test_object>>);
		using Allocator = std::pmr::polymorphic_allocator<test_object>;

		concurrent_queue<test_object, std::deque, Allocator> a{'a', 'b', 'c'};

		test_object::clearfunctioncalls();
		concurrent_queue b(std::move(a), Allocator{});
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_TRUE((std::is_same_v<std::pmr::polymorphic_allocator<test_object>, decltype(a.get_allocator())>) );
		EXPECT_FALSE((std::is_same_v<std::allocator<test_object>, decltype(a.get_allocator())>) );

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);

		EXPECT_FALSE(b.empty());
		EXPECT_EQ(b.size(), 3);

		test_object tmp;
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "a");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "b");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "c");
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, initializer_list_constructor)
	{
		test_object::clearfunctioncalls();
		concurrent_queue<test_object> a{"t", "a", "r"};
		EXPECT_EQ(3, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(3, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_FALSE(a.empty());
		EXPECT_EQ(a.size(), 3);

		test_object tmp;
		EXPECT_TRUE(a.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "t");
		EXPECT_TRUE(a.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "a");
		EXPECT_TRUE(a.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "r");
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, initializer_list_constructor_with_allocator)
	{
		using Allocator = std::pmr::polymorphic_allocator<test_object>;

		test_object::clearfunctioncalls();
		concurrent_queue<test_object, std::deque, Allocator> b{"n", "i", "c"};
		EXPECT_EQ(3, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(3, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_FALSE(b.empty());
		EXPECT_EQ(b.size(), 3);

		EXPECT_TRUE((std::is_same_v<std::pmr::polymorphic_allocator<test_object>, decltype(b.get_allocator())>) );
		EXPECT_FALSE((std::is_same_v<std::allocator<test_object>, decltype(b.get_allocator())>) );

		test_object tmp;
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "n");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "i");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "c");
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, copy_assignment)
	{
		EXPECT_TRUE(std::is_copy_assignable_v<concurrent_queue<int>>);
		EXPECT_TRUE((std::is_assignable_v<concurrent_queue<int>, concurrent_queue<int>>) );

		test_object                   d('d');
		concurrent_queue<test_object> a(4, d);

		test_object::clearfunctioncalls();
		concurrent_queue<test_object> b = a;

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(4, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_EQ(a, b);
		EXPECT_EQ(b.size(), 4);

		for (auto itr = b.cbegin(); itr < b.cend(); ++itr)
		{
			EXPECT_EQ(*itr, d);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, move_assignment)
	{
		EXPECT_TRUE(std::is_move_assignable_v<concurrent_queue<int>>);

		concurrent_queue<test_object> a{'a', 'b', 'c'};

		test_object::clearfunctioncalls();
		concurrent_queue<test_object> b = std::move(a);
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);

		EXPECT_FALSE(b.empty());
		EXPECT_EQ(b.size(), 3);

		test_object tmp;
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "a");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "b");
		EXPECT_TRUE(b.try_pop(tmp));
		EXPECT_STREQ(tmp.getString().data(), "c");
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, move_and_swap_exception_contracts)
	{
		using underlying_type      = throwing_move_queue<int, std::allocator<int>>;
		using queue_type           = concurrent_queue<int, throwing_move_queue>;
		using allocator_type       = stateful_test_allocator<int>;
		using allocated_queue_type = concurrent_queue<int, throwing_move_queue, allocator_type>;

		static_assert(!std::is_nothrow_move_constructible_v<queue_type>);
		static_assert(!std::is_nothrow_move_assignable_v<queue_type>);
		static_assert(!noexcept(allocated_queue_type(std::declval<allocated_queue_type&&>(), allocator_type{})));
		static_assert(!noexcept(swap(std::declval<queue_type&>(), std::declval<queue_type&>())));

		queue_type source{1, 2, 3};
		underlying_type::throw_on_move = true;
		EXPECT_THROW((queue_type(std::move(source))), std::runtime_error);
		underlying_type::throw_on_move = false;
		EXPECT_EQ(source.size(), 3);

		allocated_queue_type allocated_source({6, 7, 8}, allocator_type{1});
		throwing_move_queue<int, allocator_type>::throw_on_move = true;
		EXPECT_THROW((allocated_queue_type(std::move(allocated_source), allocator_type{2})), std::runtime_error);
		throwing_move_queue<int, allocator_type>::throw_on_move = false;
		EXPECT_EQ(allocated_source.size(), 3);

		queue_type move_target{4, 5};
		underlying_type::throw_on_move = true;
		EXPECT_THROW(move_target = std::move(source), std::runtime_error);
		underlying_type::throw_on_move = false;
		EXPECT_EQ(source.size(), 3);
		EXPECT_EQ(move_target.size(), 2);

		underlying_type::throw_on_move = true;
		EXPECT_THROW(swap(source, move_target), std::runtime_error);
		underlying_type::throw_on_move = false;
		EXPECT_EQ(source.size(), 3);
		EXPECT_EQ(move_target.size(), 2);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, self_assignment)
	{
		concurrent_queue<int> queue{1, 2, 3};

		queue = queue;
		EXPECT_EQ(queue, (concurrent_queue<int>{1, 2, 3}));

		queue = std::move(queue);
		EXPECT_EQ(queue, (concurrent_queue<int>{1, 2, 3}));
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, assignment_operators_replace_contents)
	{
		concurrent_queue<int> source{1, 2, 3};
		concurrent_queue<int> copy_target{9};
		copy_target = source;
		EXPECT_EQ(copy_target, source);

		concurrent_queue<int> move_target{8, 9};
		move_target = std::move(copy_target);
		EXPECT_EQ(move_target, source);
		EXPECT_TRUE(copy_target.empty());
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, supports_non_random_access_underlying_queue)
	{
		concurrent_queue<int, std::list> lhs{1, 2, 3};
		concurrent_queue<int, std::list> rhs{1, 2, 3};
		concurrent_queue<int, std::list> greater{1, 2, 4};

		EXPECT_EQ(lhs, rhs);
		EXPECT_EQ(lhs <=> rhs, std::strong_ordering::equal);
		EXPECT_LT(lhs, greater);
		EXPECT_GT(greater, rhs);
		swap(lhs, rhs);
		EXPECT_EQ(lhs, rhs);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, acquire_read_lock)
	{
		// make sure the lock can outlive the class without crashing
		concurrent_queue<int>::read_lock_type lock;

		{
			const concurrent_queue a{1, 2};
			lock = std::move(a.acquire_read_lock());
		}

		lock.unlock();
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, acquire_write_lock)
	{
		// make sure the lock can outlive the class without crashing
		concurrent_queue<int>::write_lock_type lock;

		{
			concurrent_queue<int> a{1, 2};
			lock = std::move(a.acquire_write_lock());
		}

		lock.unlock();
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, clear)
	{
		concurrent_queue<test_object> a{'n', 'i', 'c'};

		EXPECT_FALSE(a.empty());
		EXPECT_EQ(a.size(), 3);

		test_object::clearfunctioncalls();
		a.clear();
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, emplace)
	{
		concurrent_queue<test_object> a;

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);

		test_object::clearfunctioncalls();
		a.emplace("test");
		EXPECT_EQ(1, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_FALSE(a.empty());
		EXPECT_EQ(a.size(), 1);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, emplace_threads)
	{
		concurrent_queue<int> a;
		std::barrier          iteration{2};

		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		// create 1000 numbers split across odd/even vectors
		std::vector<int> odds(500);
		std::vector<int> evens(500);

		std::generate_n(odds.begin(), odds.size(), generate_odd_numbers{});
		std::generate_n(evens.begin(), evens.size(), generate_even_numbers{});

		EXPECT_EQ(odds.front(), 1);
		EXPECT_EQ(odds.back(), 999);
		EXPECT_EQ(evens.front(), 2);
		EXPECT_EQ(evens.back(), 1000);

		// Synchronize every iteration so both producers remain active for the entire workload.
		auto even_thread = std::async(std::launch::async, [evens, &a, &iteration]
		                              {
			                              for (int i : evens)
			                              {
				                              iteration.arrive_and_wait();
				                              a.emplace(i);
			                              }
			                              return std::this_thread::get_id(); });
		auto odd_thread  = std::async(std::launch::async, [odds, &a, &iteration]
                                     {
                                         for (int i : odds)
                                         {
				                             iteration.arrive_and_wait();
				                             a.emplace(i);
                                         }
                                         return std::this_thread::get_id(); });

		auto odd_thread_id  = odd_thread.get();
		auto even_thread_id = even_thread.get();

		// ensure 3 different threads
		EXPECT_NE(main_thread_id, odd_thread_id);
		EXPECT_NE(main_thread_id, even_thread_id);
		EXPECT_NE(odd_thread_id, even_thread_id);

		// there should be 1000 total elements
		EXPECT_EQ(a.size(), 1000);

		// if we then sort the elements, we should have all the numbers from 1 to 1000 with no duplicates.
		std::ranges::sort(a);

		int i = 0;
		for (auto itr = a.cbegin(); itr != a.cend(); ++itr)
			EXPECT_EQ(*itr, ++i);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, empty)
	{
		concurrent_queue<test_object> a{'n', 'i', 'c'};

		EXPECT_FALSE(a.empty());
		EXPECT_EQ(a.size(), 3);

		test_object::clearfunctioncalls();
		a.clear();
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, get_allocator)
	{
		concurrent_queue<int> a;
		EXPECT_EQ(a.get_allocator(), std::allocator<int>{});
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, push_lvalue)
	{
		concurrent_queue<test_object> a;
		test_object                   t("test_object"s);

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);

		test_object::clearfunctioncalls();
		a.push(t);
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(1, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_FALSE(a.empty());
		EXPECT_EQ(a.size(), 1);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, push_lvalue_threads)
	{
		concurrent_queue<int> a;
		std::barrier          iteration{2};

		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		// create 1000 numbers split across odd/even vectors
		std::vector<int> odds(500);
		std::vector<int> evens(500);

		std::generate_n(odds.begin(), odds.size(), generate_odd_numbers{});
		std::generate_n(evens.begin(), evens.size(), generate_even_numbers{});

		EXPECT_EQ(odds.front(), 1);
		EXPECT_EQ(odds.back(), 999);
		EXPECT_EQ(evens.front(), 2);
		EXPECT_EQ(evens.back(), 1000);

		// Synchronize every iteration so both producers remain active for the entire workload.
		auto even_thread = std::async(std::launch::async, [evens, &a, &iteration]
		                              {
			                              for (int i : evens)
			                              {
				                              iteration.arrive_and_wait();
				                              a.push(i);
			                              }
			                              return std::this_thread::get_id(); });
		auto odd_thread  = std::async(std::launch::async, [odds, &a, &iteration]
                                     {
                                         for (int i : odds)
                                         {
				                             iteration.arrive_and_wait();
				                             a.push(i);
                                         }
                                         return std::this_thread::get_id(); });

		auto odd_thread_id  = odd_thread.get();
		auto even_thread_id = even_thread.get();

		// ensure 3 different threads
		EXPECT_NE(main_thread_id, odd_thread_id);
		EXPECT_NE(main_thread_id, even_thread_id);
		EXPECT_NE(odd_thread_id, even_thread_id);

		// there should be 1000 total elements
		EXPECT_EQ(a.size(), 1000);

		// if we then sort the elements, we should have all the numbers from 1 to 1000 with no duplicates.
		std::ranges::sort(a);

		int i = 0;
		for (auto itr = a.cbegin(); itr != a.cend(); ++itr)
			EXPECT_EQ(*itr, ++i);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, push_rvalue)
	{
		// should not deadlock, `a` should be in its original state
		concurrent_queue<test_object> a;
		test_object                   t("test_object"s);

		EXPECT_TRUE(a.empty());
		EXPECT_EQ(a.size(), 0);

		test_object::clearfunctioncalls();
		a.push(std::move(t));

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(1, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_FALSE(a.empty());
		EXPECT_EQ(a.size(), 1);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop)
	{
		// should not deadlock, `a` should have its head element removed
		concurrent_queue<int> a({1, 2, 3});
		concurrent_queue<int> b({2, 3});
		int                   result = 0;

		EXPECT_NE(a, b);
		EXPECT_EQ(result, 0);

		EXPECT_TRUE(a.try_pop(result));
		EXPECT_EQ(result, 1);

		EXPECT_EQ(a, b);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_threads)
	{
		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		// create 1000 numbers
		std::vector<int> numbers(1000);
		std::iota(numbers.begin(), numbers.end(), 0);

		EXPECT_EQ(numbers.front(), 0);
		EXPECT_EQ(numbers.back(), 999);

		concurrent_queue<int> a(numbers.begin(), numbers.end());

		int          count1 = 0;
		int          count2 = 0;
		std::barrier iteration{2};

		// Each consumer wins one contested pop per round. This proves sustained overlap without scheduler sleeps.
		auto thread1 = std::async(std::launch::async, [&a, &count1, &iteration]
		                          {
			                          int val;
			                          for (int i = 0; i < 500; ++i)
			                          {
				                          iteration.arrive_and_wait();
				                          while (!a.try_pop(val))
					                          std::this_thread::yield();
				                          ++count1;
				                          iteration.arrive_and_wait();
			                          }
			                          return std::this_thread::get_id(); });
		auto thread2 = std::async(std::launch::async, [&a, &count2, &iteration]
		                          {
			                          int val;
			                          for (int i = 0; i < 500; ++i)
			                          {
				                          iteration.arrive_and_wait();
				                          while (!a.try_pop(val))
					                          std::this_thread::yield();
				                          ++count2;
				                          iteration.arrive_and_wait();
			                          }
			                          return std::this_thread::get_id(); });

		auto thread1_id = thread1.get();
		auto thread2_id = thread2.get();

		// ensure 3 different threads
		EXPECT_NE(main_thread_id, thread1_id);
		EXPECT_NE(main_thread_id, thread2_id);
		EXPECT_NE(thread1_id, thread2_id);

		// check that both threads popped a similar number of elements. This implies they ran at the same time, otherwise
		// one thread would have popped them all
		EXPECT_EQ(count1, 500);
		EXPECT_EQ(count2, 500);
		EXPECT_EQ(count1 + count2, 1000);

		// there should be 1000 total elements
		EXPECT_EQ(a.size(), 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_empty)
	{
		// should not deadlock, `a` should have its head element removed
		concurrent_queue<int> a;
		int                   result = 3;

		EXPECT_EQ(result, 3);

		EXPECT_FALSE(a.try_pop(result));
		EXPECT_EQ(result, 3);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_locked)
	{
		// should not deadlock, `a` should have its head element removed
		concurrent_queue<int> a{42};
		EXPECT_EQ(a.size(), 1);

		int result = 0;

		auto lock = a.acquire_read_lock();

		EXPECT_FALSE(a.try_pop(result));
		EXPECT_EQ(a.size(), 1);
		EXPECT_EQ(result, 0);

		lock.unlock();

		EXPECT_TRUE(a.try_pop(result));
		EXPECT_EQ(a.size(), 0);
		EXPECT_EQ(result, 42);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_empty_threads)
	{
		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		concurrent_queue<int> a;

		int count1 = 0;
		int count2 = 0;

		// Make sure the thread loops have at least a 1ms sleep in them... otherwise it seems like the compiler vectorizes
		// the loops and runs them sequentially. Not bad as an optimization, but it defeats the purpose of testing two
		// competing threads banging on the concurrent_queue
		auto thread1 = std::async(std::launch::async, [&a, &count1]
		                          {
			                          int val;
			                          for (int i = 0; i < 1000; ++i)
			                          {
				                          if (a.try_pop(val))
					                          ++count1;
			                          }
			                          return std::this_thread::get_id(); });
		auto thread2 = std::async(std::launch::async, [&a, &count2]
		                          {
			                          int val;
			                          for (int i = 0; i < 1000; ++i)
			                          {
				                          if (a.try_pop(val))
					                          ++count2;
			                          }
			                          return std::this_thread::get_id(); });

		auto thread1_id = thread1.get();
		auto thread2_id = thread2.get();

		// ensure 3 different threads
		EXPECT_NE(main_thread_id, thread1_id);
		EXPECT_NE(main_thread_id, thread2_id);

		// check that both threads popped a similar number of elements. This implies they ran at the same time, otherwise
		// one thread would have popped them all
		EXPECT_EQ(count1, 0);
		EXPECT_EQ(count2, 0);

		// there should be 1000 total elements
		EXPECT_EQ(a.size(), 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_for)
	{
		// should not deadlock, `a` should have its head element removed
		concurrent_queue<int> a({1, 2, 3});
		concurrent_queue<int> b({2, 3});
		int                   result = 0;

		EXPECT_NE(a, b);
		EXPECT_EQ(result, 0);

		EXPECT_TRUE(a.try_pop_for(result, 1ms));
		EXPECT_EQ(result, 1);

		EXPECT_EQ(a, b);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_for_empty)
	{
		// should not deadlock, `a` should be in its original state
		concurrent_queue<int>  a;
		concurrent_queue<int>  b;
		constexpr microseconds timeout = 100ms;
		int                    result  = 0;

		EXPECT_EQ(a, b);
		EXPECT_EQ(result, 0);

		const auto start = steady_clock::now();
		EXPECT_FALSE(a.try_pop_for(result, timeout));
		microseconds elapsed = duration_cast<microseconds>(steady_clock::now() - start);

		// expect the timeout to be +/- 10% of the requested value
		EXPECT_LT(abs(timeout - elapsed).count(), (0.15 * timeout).count());
		EXPECT_EQ(result, 0);

		EXPECT_EQ(a, b);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_for_threads)
	{
		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		// create 1000 numbers
		std::vector<int> numbers(1000);
		std::iota(numbers.begin(), numbers.end(), 0);

		EXPECT_EQ(numbers.front(), 0);
		EXPECT_EQ(numbers.back(), 999);

		concurrent_queue<int> a(numbers.begin(), numbers.end());

		int count1 = 0;
		int count2 = 0;

		// Make sure the thread loops have at least a 1ms sleep in them... otherwise it seems like the compiler vectorizes
		// the loops and runs them sequentially. Not bad as an optimization, but it defeats the purpose of testing two
		// competing threads banging on the concurrent_queue
		auto thread1 = std::async(std::launch::async, [&a, &count1]
		                          {
			                          int val;
			                          while (!a.empty())
			                          {
				                          if (a.try_pop_for(val, 1ms))
					                          ++count1;
			                          }
			                          return std::this_thread::get_id(); });
		auto thread2 = std::async(std::launch::async, [&a, &count2]
		                          {
			                          int val;
			                          while (!a.empty())
			                          {
				                          if (a.try_pop_for(val, 1ms))
					                          ++count2;
			                          }
			                          return std::this_thread::get_id(); });

		auto thread1_id = thread1.get();
		auto thread2_id = thread2.get();

		// ensure 3 different threads
		EXPECT_NE(main_thread_id, thread1_id);
		EXPECT_NE(main_thread_id, thread2_id);
		EXPECT_NE(thread1_id, thread2_id);

		// check that both threads popped a similar number of elements. This implies they ran at the same time, otherwise
		// one thread would have popped them all
		EXPECT_GT(count1, 100);
		EXPECT_GT(count2, 100);
		EXPECT_EQ(count1 + count2, 1000);

		// there should be 1000 total elements
		EXPECT_EQ(a.size(), 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, try_pop_for_timeout)
	{
		// should not deadlock, `a` should be in its original state
		concurrent_queue<int> a{1, 2, 3};
		auto                  lock    = a.acquire_read_lock();
		microseconds          timeout = 100ms;

		int  result = 0;
		auto start  = steady_clock::now();
		EXPECT_FALSE(a.try_pop_for(result, 100ms));
		microseconds elapsed = duration_cast<microseconds>(steady_clock::now() - start);

		// timeout was +/- 10% of the requested value. This is probably only reasonable for values in the 100+ ms range.
		EXPECT_LT(abs(timeout - elapsed).count(), (0.15 * timeout).count());
		EXPECT_EQ(result, 0);
		EXPECT_EQ(a.size(), 3);

		lock.unlock();
		EXPECT_TRUE(a.try_pop_for(result, 1ms));
		EXPECT_EQ(result, 1);
		EXPECT_EQ(a.size(), 2);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, unsafe_iteration)
	{
		concurrent_queue<int> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		int                   i = 0;

		// `begin()` and `end()` functions are called by range loops
		for (auto value : a)
			EXPECT_EQ(++i, value);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, unsafe_const_iteration)
	{
		const concurrent_queue<int> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		int                         i = 0;

		// `begin()` and `end()` functions are called by range loops
		for (const auto& value : a)
			EXPECT_EQ(++i, value);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, unsafeSize)
	{
		const concurrent_queue<int> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		EXPECT_EQ(a.size(), 10);

		const concurrent_queue<int> b(101);
		EXPECT_EQ(b.size(), 101);

		const concurrent_queue<int> c;
		EXPECT_EQ(c.size(), 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, swap)
	{
		concurrent_queue<test_object> a{'a', 'b', 'c'};
		concurrent_queue<test_object> b{'l', 'm', 'n', 'o', 'p'};

		EXPECT_EQ(a.size(), 3);
		EXPECT_EQ(b.size(), 5);

		test_object::clearfunctioncalls();
		std::swap(a, b);
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_EQ(a.size(), 5);
		EXPECT_EQ(b.size(), 3);

		EXPECT_EQ(a, (concurrent_queue<test_object>{'l', 'm', 'n', 'o', 'p'}));
		EXPECT_EQ(b, (concurrent_queue<test_object>{'a', 'b', 'c'}));
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, self_swap)
	{
		// should not deadlock
		concurrent_queue<test_object> a{'a', 'b', 'c'};

		EXPECT_EQ(a.size(), 3);

		test_object::clearfunctioncalls();
		swap(a, a);
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());

		EXPECT_EQ(a.size(), 3);

		EXPECT_EQ(a, (concurrent_queue<test_object>{'a', 'b', 'c'}));
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, equality)
	{
		concurrent_queue a{1, 2, 3};
		concurrent_queue b{1, 2, 3};
		concurrent_queue c{4, 5, 6};

		EXPECT_EQ(a, b);
		EXPECT_NE(a, c);
		EXPECT_EQ(a, a);
		EXPECT_NE(a, (concurrent_queue<int>{1, 2}));
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, producer_consumer)
	{
		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		// create 100 numbers
		constexpr int    test_size = 100;
		std::vector<int> numbers_in(test_size);
		std::vector<int> numbers_out;
		std::iota(numbers_in.begin(), numbers_in.end(), 0);

		EXPECT_EQ(numbers_in.front(), 0);
		EXPECT_EQ(numbers_in.back(), test_size - 1);
		EXPECT_EQ(numbers_in.size(), test_size);
		EXPECT_EQ(numbers_out.size(), 0);

		concurrent_queue<int> queue;
		std::barrier          iteration{2};

		// Pair every production/consumption round while still racing the push against the blocking pop.
		auto producer = std::async(std::launch::async, [&queue, &numbers_in, &iteration]
		                           {
			                           for (int i : numbers_in)
			                           {
				                           iteration.arrive_and_wait();
				                           queue.push(i);
				                           iteration.arrive_and_wait();
			                           }
			                           return std::this_thread::get_id(); });
		auto consumer = std::async(std::launch::async, [&queue, &numbers_out, test_size, &iteration]
		                           {
			                           int val = 0;
			                           for (int i = 0; i < test_size; ++i)
			                           {
				                           iteration.arrive_and_wait();
				                           if (queue.try_pop_for(val, 1s))
					                           numbers_out.push_back(val);
				                           else
					                           ADD_FAILURE() << "timed out waiting for item " << i;
				                           iteration.arrive_and_wait();
			                           }
			                           return std::this_thread::get_id(); });

		auto producer_id = producer.get();
		auto consumer_id = consumer.get();

		// ensure 3 different threads
		EXPECT_NE(main_thread_id, producer_id);
		EXPECT_NE(main_thread_id, consumer_id);
		EXPECT_NE(producer_id, consumer_id);

		// check that all the numbers from in are now in out
		EXPECT_EQ(numbers_out.front(), 0);
		EXPECT_EQ(numbers_out.back(), test_size - 1);
		EXPECT_EQ(numbers_out.size(), test_size);
		EXPECT_TRUE(std::ranges::is_sorted(numbers_out));    // given the first, last, and size are correct, this tells us all the rest of the values are also correct
	}

	//------------------------------------------------------------------------------------------------------------------
	TEST_F(concurrent_queue_test, multiple_producer_consumer)
	{
		// ensure we actually have different threads
		std::thread::id main_thread_id = std::this_thread::get_id();

		// create 200 numbers split across odd/even vectors
		std::deque<int> odds(100);
		std::deque<int> evens(100);
		std::deque<int> out1;
		std::deque<int> out2;

		std::generate_n(odds.begin(), odds.size(), generate_odd_numbers{});
		std::generate_n(evens.begin(), evens.size(), generate_even_numbers{});

		EXPECT_EQ(odds.front(), 1);
		EXPECT_EQ(odds.back(), 199);
		EXPECT_EQ(evens.front(), 2);
		EXPECT_EQ(evens.back(), 200);
		EXPECT_EQ(out1.size(), 0);
		EXPECT_EQ(out2.size(), 0);

		concurrent_queue<int> queue;
		std::barrier          iteration{4};

		// Every round has two simultaneous producers and two blocking consumers.
		auto producer1 = std::async(std::launch::async, [&queue, &odds, &iteration]
		                            {
			                            for (int i = 0; i < 100; ++i)
			                            {
				                            iteration.arrive_and_wait();
				                            queue.push(odds.front());
				                            odds.pop_front();
				                            iteration.arrive_and_wait();
			                            }
			                            return std::this_thread::get_id(); });
		auto producer2 = std::async(std::launch::async, [&queue, &evens, &iteration]
		                            {
			                            for (int i = 0; i < 100; ++i)
			                            {
				                            iteration.arrive_and_wait();
				                            queue.push(evens.front());
				                            evens.pop_front();
				                            iteration.arrive_and_wait();
			                            }
			                            return std::this_thread::get_id(); });
		auto consumer1 = std::async(std::launch::async, [&queue, &out1, &iteration]
		                            {
			                            int val = 0;
			                            for (int i = 0; i < 100; ++i)
			                            {
				                            iteration.arrive_and_wait();
				                            if (queue.try_pop_for(val, 1s))
					                            out1.push_back(val);
				                            else
					                            ADD_FAILURE() << "consumer 1 timed out on iteration " << i;
				                            iteration.arrive_and_wait();
			                            }
			                            return std::this_thread::get_id(); });
		auto consumer2 = std::async(std::launch::async, [&queue, &out2, &iteration]
		                            {
			                            int val = 0;
			                            for (int i = 0; i < 100; ++i)
			                            {
				                            iteration.arrive_and_wait();
				                            if (queue.try_pop_for(val, 1s))
					                            out2.push_back(val);
				                            else
					                            ADD_FAILURE() << "consumer 2 timed out on iteration " << i;
				                            iteration.arrive_and_wait();
			                            }
			                            return std::this_thread::get_id(); });

		EXPECT_NE(producer1.get(),std::thread::id{});
		EXPECT_NE(producer2.get(),std::thread::id{});
		EXPECT_NE(consumer1.get(),std::thread::id{});
		EXPECT_NE(consumer2.get(),std::thread::id{});

		// check that all the numbers from in are now in out
		EXPECT_EQ(odds.size(), 0);
		EXPECT_EQ(evens.size(), 0);
		EXPECT_EQ(out1.size(), 100);
		EXPECT_EQ(out2.size(), 100);
		EXPECT_EQ(out1.size() + out2.size(), 200);

		std::deque<int> full_output;

		// merge needs sorted input, produces sorted output
		std::ranges::sort(out1);
		std::ranges::sort(out2);
		std::ranges::merge(std::as_const(out1), std::as_const(out2), std::back_inserter(full_output));

		EXPECT_EQ(full_output.size(), 200);

		int i = 0;
		for (int value : full_output)
			EXPECT_EQ(value, ++i);
	}
}    // namespace
