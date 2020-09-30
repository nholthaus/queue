//#define TEST_OBJECT_OUTPUT	// for this to work, has to be defined before test_object.h is included.

#include <circular_queue.h>
#include <gtest/gtest.h>

#include <cassert>
#include <chrono>
#include <deque>
#include <memory>
#include <type_traits>
#include <vector>

namespace
{

	/// Wrapper class for checking out the private data members of the queue for test purposes
	class circular_queue_test_wrapper : public circular_queue<test_object>
	{
	public:
		circular_queue_test_wrapper(size_t capacity)
		    : circular_queue(capacity){};
		test_object* data() const
		{
			return m_data;
		}
		test_object* head() const
		{
			return m_head.ptr;
		}
		test_object* tail() const
		{
			return m_tail.ptr;
		}
		bool headParity() const
		{
			return m_head.parity;
		}
		bool tailParity() const
		{
			return m_tail.parity;
		}
	};

	// The fixture for testing class Foo.
	class circular_queue_test : public ::testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is empty.

		circular_queue_test()
		{
			// You can do set-up work for each test here.
		}

		virtual ~circular_queue_test()
		{
			// You can do clean-up work that doesn't throw exceptions here.
		}

		// If the constructor and destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code here will be called immediately after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code here will be called immediately after each test (right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	// The fixture for testing iterators.
	// These tests assume that the circular_queue members work!
	class circular_queue_iterator_test : public ::testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is empty.

		circular_queue_iterator_test()
		{
			emptyBuffer        = new circular_queue_test_wrapper(16);
			partialBuffer      = new circular_queue_test_wrapper(16);
			fullBuffer         = new circular_queue_test_wrapper(16);
			fullOverflowBuffer = new circular_queue_test_wrapper(16);

			partialBuffer->emplace_back('a');    // start
			partialBuffer->emplace_back('b');
			partialBuffer->emplace_back('c');
			partialBuffer->emplace_back('d');    // end

			fullBuffer->emplace_back('a');    // start
			fullBuffer->emplace_back('b');
			fullBuffer->emplace_back('c');
			fullBuffer->emplace_back('d');
			fullBuffer->emplace_back('e');
			fullBuffer->emplace_back('f');
			fullBuffer->emplace_back('g');
			fullBuffer->emplace_back('h');
			fullBuffer->emplace_back('i');
			fullBuffer->emplace_back('j');
			fullBuffer->emplace_back('k');
			fullBuffer->emplace_back('l');
			fullBuffer->emplace_back('m');
			fullBuffer->emplace_back('n');
			fullBuffer->emplace_back('o');
			fullBuffer->emplace_back('p');    // end

			fullOverflowBuffer->emplace_back('a');
			fullOverflowBuffer->emplace_back('b');
			fullOverflowBuffer->emplace_back('c');
			fullOverflowBuffer->emplace_back('d');
			fullOverflowBuffer->emplace_back('e');
			fullOverflowBuffer->emplace_back('f');
			fullOverflowBuffer->emplace_back('g');
			fullOverflowBuffer->emplace_back('h');
			fullOverflowBuffer->emplace_back('i');
			fullOverflowBuffer->emplace_back('j');
			fullOverflowBuffer->emplace_back('k');    // start
			fullOverflowBuffer->emplace_back('l');
			fullOverflowBuffer->emplace_back('m');
			fullOverflowBuffer->emplace_back('n');
			fullOverflowBuffer->emplace_back('o');
			fullOverflowBuffer->emplace_back('p');
			fullOverflowBuffer->emplace_back('q');
			fullOverflowBuffer->emplace_back('r');
			fullOverflowBuffer->emplace_back('s');
			fullOverflowBuffer->emplace_back('t');
			fullOverflowBuffer->emplace_back('u');
			fullOverflowBuffer->emplace_back('v');
			fullOverflowBuffer->emplace_back('w');
			fullOverflowBuffer->emplace_back('x');
			fullOverflowBuffer->emplace_back('y');
			fullOverflowBuffer->emplace_back('z');    // end

			partialOverflowBuffer = new circular_queue_test_wrapper(*fullOverflowBuffer);
			partialOverflowBuffer->emplace_back('a');
			partialOverflowBuffer->emplace_back('b');
			partialOverflowBuffer->emplace_back('c');
			partialOverflowBuffer->emplace_back('d');
			partialOverflowBuffer->emplace_back('e');
			partialOverflowBuffer->emplace_back('f');
			partialOverflowBuffer->emplace_back('g');
			partialOverflowBuffer->emplace_back('h');
			partialOverflowBuffer->emplace_back('i');
			partialOverflowBuffer->emplace_back('j');
			partialOverflowBuffer->emplace_back('k');
			partialOverflowBuffer->emplace_back('l');
			partialOverflowBuffer->emplace_back('m');
			partialOverflowBuffer->emplace_back('n');    // start
			partialOverflowBuffer->emplace_back('o');
			partialOverflowBuffer->emplace_back('p');
			partialOverflowBuffer->emplace_back('q');
			partialOverflowBuffer->emplace_back('r');
			partialOverflowBuffer->emplace_back('s');
			partialOverflowBuffer->emplace_back('t');
			partialOverflowBuffer->emplace_back('u');
			partialOverflowBuffer->emplace_back('v');
			partialOverflowBuffer->emplace_back('w');    // end
			partialOverflowBuffer->emplace_back('x');
			partialOverflowBuffer->emplace_back('y');
			partialOverflowBuffer->emplace_back('z');
			partialOverflowBuffer->pop_front();
			partialOverflowBuffer->pop_front();
			partialOverflowBuffer->pop_front();
			partialOverflowBuffer->pop_back();
			partialOverflowBuffer->pop_back();
			partialOverflowBuffer->pop_back();
		}

		virtual ~circular_queue_iterator_test()
		{
			// You can do clean-up work that doesn't throw exceptions here.
           delete emptyBuffer;
           delete partialBuffer;
           delete fullBuffer;
           delete fullOverflowBuffer;
		   delete partialOverflowBuffer;
		}

		circular_queue_test_wrapper* emptyBuffer;
		circular_queue_test_wrapper* partialBuffer;
		circular_queue_test_wrapper* fullBuffer;
		circular_queue_test_wrapper* partialOverflowBuffer;
		circular_queue_test_wrapper* fullOverflowBuffer;
	};

	// Tests that the circular_queue constructor works, and allocates capacitys that are powers of 2
	TEST_F(circular_queue_test, Constructor)
	{
		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(256);
			EXPECT_EQ(256, buffer.capacity());

			circular_queue<test_object> buffer2(1000);
			EXPECT_EQ(1000, buffer2.capacity());

			circular_queue<test_object> buffer3(1024);
			EXPECT_EQ(1024, buffer3.capacity());

			circular_queue<test_object> buffer4(1000000);
			EXPECT_EQ(1000000, buffer4.capacity());

			size_t maxcapacity = buffer4.max_size();
			// Check that we get a bad alloc if we request too much
			EXPECT_THROW(circular_queue<test_object> buffer5(maxcapacity + 1), std::bad_alloc);

			// Check that we get a bad alloc if we request too little
			EXPECT_THROW(circular_queue<test_object> buffer7(0), std::bad_alloc);
		}

		// just allocate memory, no construction
		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(0, test_object::getstringfunctioncalls());
	}

	// tests that a single object can be emplaced on the back
	TEST_F(circular_queue_test, emplaceBack)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(16);
			EXPECT_EQ(16, buffer.capacity());

			EXPECT_EQ(0, buffer.size());
			buffer.emplace_back('a');
			EXPECT_EQ(1, buffer.size());

			// verify that if the objects constructor fails,
			// the queue throws a bad_alloc, and that the tail position (i.e. container state)
			// isn't changed.
			// also, remembar a failed constructor's destructor will NOT be called!
			test_object* tailPosition = buffer.tail();
			EXPECT_THROW(buffer.emplace_back('b', test_object::Constructor);, std::bad_alloc);
			EXPECT_EQ(tailPosition, buffer.tail());
		}

		// just one construction/destruction
		EXPECT_EQ(2, test_object::getconstructorcalls());
		EXPECT_EQ(1, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(0, test_object::getstringfunctioncalls());
	}

	// Overflow back fills a capacity 16 buffer with the 26 character lowercase alphabet.
	// it's intended to test that emplace only calls one constructor per object, and places the
	// the created objects appropriately in the buffer. DOESN'T CHECK ITERATORS.
	TEST_F(circular_queue_test, emplaceBackOverflow)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());
		int numCreated = 0;

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(16);
			EXPECT_EQ(16, buffer.capacity());
			EXPECT_TRUE(buffer.empty());
			EXPECT_FALSE(buffer.full());

			int expectedsize = 0;
			for (char ch = 'a'; ch <= 'z'; ++ch)
			{
				buffer.emplace_back(std::move(ch));

				numCreated++;

				// shouldn't be empty, should be full after 16 things have been emplaced.
				if (++expectedsize < 16)
				{
					EXPECT_EQ(expectedsize, buffer.size());
					EXPECT_FALSE(buffer.empty());
					EXPECT_FALSE(buffer.full());
				}
				else
				{
					// full
					EXPECT_EQ(16, buffer.size());
					EXPECT_FALSE(buffer.empty());
					EXPECT_TRUE(buffer.full());
				}
			}

			// for 16 capacity buffer, after writing alphabet output should be:
			// qrstuvwxyzklmnop
			std::string output[16] = {"q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "k", "l", "m", "n", "o", "p"};

			// buffer is full, head and tail should both point to"k'
			test_object* head = buffer.head();
			test_object* tail = buffer.tail();
			EXPECT_STREQ(head->getString().c_str(), "k");
			EXPECT_STREQ(tail->getString().c_str(), "k");

			// check the output is as expected
			test_object* data = buffer.data();
			for (int i = 0; i < 16; ++i)
			{
				EXPECT_STREQ(data[i].getString().c_str(), output[i].c_str());
			}
		}

		// just one construction per emplace (and one destruct!!!!!!!)
		EXPECT_EQ(numCreated, test_object::getconstructorcalls());
		EXPECT_EQ(numCreated, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(18, test_object::getstringfunctioncalls());
	}

	// Overflows the buffer several times, to verify that all the head/tail management internally
	// keeps working. Additionally, tests operator [] pretty thuroughly.
	TEST_F(circular_queue_test, emplaceBackMultiOverflow)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());
		int numCreated = 52;

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(8);
			EXPECT_EQ(8, buffer.capacity());

			// push ints in order, 6.5x buffer capacity to make sure it wraps a lot in a complicated
			// way
			for (char i = 'a'; i <= 'z'; i++)
			{
				buffer.emplace_back(std::move(i));
			}
			for (char i = 'A'; i <= 'Z'; i++)
			{
				buffer.emplace_back(std::move(i));
			}

			// check that head/tail both point to 'w'
			EXPECT_STREQ(buffer.tail()->getString().c_str(), "S");
			EXPECT_STREQ(buffer.head()->getString().c_str(), "S");

			// check the rest of values are as expected ("S" to "Z")
			const char Z         = 'Z';
			char       result[2] = {Z, '\0'};
			for (int i = 0; i < buffer.capacity(); ++i)
			{
				result[0] = char(Z - buffer.capacity() + 1 + i);
				EXPECT_STREQ(result, buffer[i].getString().c_str());
			}
		}

		// just one construction per emplace (and one destruct!!!!!!!)
		EXPECT_EQ(numCreated, test_object::getconstructorcalls());
		EXPECT_EQ(numCreated, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(10, test_object::getstringfunctioncalls());
	}

	// Tests the emplace_front function for lvalues and rvalues.
	TEST_F(circular_queue_test, EmplaceFront)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// check exception cases first to see if they screw up the container state (hint: they shouldn't)
			char c;

			circular_queue_test_wrapper buffer(8);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// exception
			EXPECT_THROW(buffer.emplace_front('z', test_object::Constructor), std::bad_alloc);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(buffer.head(), buffer.data());
			EXPECT_EQ(buffer.tail(), buffer.data());
			EXPECT_EQ(1, test_object::getconstructorcalls());    // in-place construction

			// wrap buffer 2.5x with in-place constructions
			buffer.emplace_front('t');
			buffer.emplace_front('s');
			buffer.emplace_front('r');
			buffer.emplace_front('q');
			buffer.emplace_front('p');
			buffer.emplace_front('o');
			buffer.emplace_front('n');
			buffer.emplace_front('m');
			buffer.emplace_front('l');
			buffer.emplace_front('k');
			buffer.emplace_front('j');
			buffer.emplace_front('i');
			buffer.emplace_front('h');
			buffer.emplace_front('g');
			buffer.emplace_front('f');
			buffer.emplace_front('e');
			buffer.emplace_front('d');
			buffer.emplace_front('c');
			buffer.emplace_front('b');
			buffer.emplace_front('a');

			// correct size
			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// correct data
			EXPECT_STREQ("e", buffer.data()[0].getString().c_str());

			c = 'a';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(std::string(&c, 1).c_str(), itr.getString().c_str());
				c++;
			}
			EXPECT_EQ('h' + 1, c);

			EXPECT_EQ(20, test_object::getconstructorcalls());    // in-place construction
			EXPECT_EQ(12, test_object::getdestructorcalls());     // objects deleted during wrapping
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(8, test_object::getdestructorcalls());    // remaining undeleted objects
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(9, test_object::getstringfunctioncalls());
	}

	// Tests that we can pop the front of the queue, also tests 'front'
	TEST_F(circular_queue_test, PopFront)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(4);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');

			char   expectedChar = 'a';
			size_t expectedSize = 4;

			while (buffer.size() > 0)
			{
				EXPECT_EQ(4, buffer.capacity());
				EXPECT_EQ(expectedSize, buffer.size());
				EXPECT_STREQ(std::string(&(expectedChar), 1).c_str(), buffer.front().getString().c_str());
				buffer.pop_front();
				EXPECT_EQ(1, test_object::getdestructorcalls());
				expectedSize--;
				expectedChar++;
			}

			EXPECT_TRUE(buffer.empty());
		}
	}

	// Tests that we can pop the front of the queue, also tests 'back'
	TEST_F(circular_queue_test, PopBack)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(4);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');

			char   expectedChar = 'd';
			size_t expectedSize = 4;

			while (buffer.size() > 0)
			{
				EXPECT_EQ(4, buffer.capacity());
				EXPECT_EQ(expectedSize, buffer.size());
				EXPECT_STREQ(std::string(&(expectedChar), 1).c_str(), buffer.back().getString().c_str());
				buffer.pop_back();
				EXPECT_EQ(1, test_object::getdestructorcalls());
				expectedSize--;
				expectedChar--;
			}

			EXPECT_TRUE(buffer.empty());
		}
	}

	TEST_F(circular_queue_test, ComplicatedPop)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());
		int numCreated = 52;

		// extra scope to support memory leak detection
		{
		}
	}

	// Tests that the fill constructor builds a container of a given size with all the elements
	TEST_F(circular_queue_test, FillConstructor)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			test_object fillValue('a');

			circular_queue<test_object> buffer(16, fillValue);
			EXPECT_EQ(16, buffer.capacity());
			EXPECT_EQ(16, buffer.size());

			for (int i = 0; i < buffer.size(); i++)
			{
				EXPECT_STREQ(buffer[i].getString().c_str(), "a");
			}

			// throw on capacity 0
			EXPECT_THROW(circular_queue<test_object> buffer1(0, fillValue), std::bad_alloc);

			// throw on capacity greater than max
			EXPECT_THROW(circular_queue<test_object> buffer2(buffer.max_size() + 1, fillValue), std::bad_alloc);

			// throw if test_object constructor throws
			test_object failValue('a', test_object::CopyConstructor);
			EXPECT_THROW(circular_queue<test_object> buffer3(16, failValue), std::bad_alloc);
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(2, test_object::getconstructorcalls());
		EXPECT_EQ(18, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(16, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(16, test_object::getstringfunctioncalls());
	}

	// Tests that the range constructor can build a queue based off of iterators from another container
	TEST_F(circular_queue_test, RangeConstructor)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(4);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');

			std::vector<test_object> vector1;
			vector1.reserve(3);
			vector1.emplace_back('e');
			vector1.emplace_back('f');
			vector1.emplace_back('g');

			// wrong type
			std::vector<double> vector2;
			vector2.push_back(1.4142135623730950488016887242097);
			vector2.push_back(3.1415926535897932384626433832795);

			// empty vector
			std::vector<test_object> vector3;

			// un-copyable vector
			std::vector<test_object> vector4;
			vector4.emplace_back('z', test_object::CopyConstructor);

			// normal range construct
			circular_queue<test_object> buffer1(buffer.begin(), buffer.end());
			EXPECT_EQ(4, buffer.capacity());
			EXPECT_EQ(4, buffer.size());
			EXPECT_EQ(4, buffer1.capacity());
			EXPECT_EQ(4, buffer1.size());
			for (int i = 0; i < buffer.size(); i++)
			{
				EXPECT_STREQ(buffer[i].getString().c_str(), buffer1[i].getString().c_str());
			}

			// range construct from vector
			circular_queue<test_object> buffer2(vector1.begin(), vector1.end());
			EXPECT_EQ(3, vector1.capacity());
			EXPECT_EQ(3, vector1.size());
			EXPECT_EQ(3, buffer2.capacity());
			EXPECT_EQ(3, buffer2.size());
			for (int i = 0; i < buffer2.size(); i++)
			{
				EXPECT_STREQ(vector1[i].getString().c_str(), buffer2[i].getString().c_str());
			}

			// fail on wrong type
			//			circular_queue<test_object> buffer3(vector2.begin(), vector2.end());		// yup, this fails!

			// throw on capacity 0
			EXPECT_THROW(circular_queue<test_object> buffer1(vector3.begin(), vector3.end()), std::bad_alloc);

			// throw on capacity greater than max
			// -> untestable because I'd have to make another container with more than the max
			// capacity, which I can't.

			// throw if test_object constructor throws
			EXPECT_THROW(circular_queue<test_object> buffer3(vector4.begin(), vector4.end()), std::bad_alloc);
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(8, test_object::getconstructorcalls());
		EXPECT_EQ(15, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(7, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(14, test_object::getstringfunctioncalls());
	}

	// Tests that we can construct from an initializer_list
	TEST_F(circular_queue_test, InitializerListConstructor)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer{'a', 'b', 'c', 'd'};
			EXPECT_STREQ(buffer[0].getString().c_str(), "a");
			EXPECT_STREQ(buffer[1].getString().c_str(), "b");
			EXPECT_STREQ(buffer[2].getString().c_str(), "c");
			EXPECT_STREQ(buffer[3].getString().c_str(), "d");
			EXPECT_EQ(4, buffer.capacity());
			EXPECT_EQ(4, buffer.size());

			// throw on capacity 0
			EXPECT_THROW(circular_queue<test_object> buffer1{}, std::bad_alloc);

			// throw on capacity greater than max
			// -> untestable because I'd have to write an initializer list with like a file full of
			// literal values.

			// throw if test_object constructor throws
			EXPECT_THROW(circular_queue<test_object> buffer2{test_object('z', test_object::CopyConstructor)}, std::bad_alloc);
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(5, test_object::getconstructorcalls());
		EXPECT_EQ(9, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(4, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(4, test_object::getstringfunctioncalls());
	}

	// Tests that the copy constructor makes a deep-copy of the other object. Test objects hold
	// strings so this is a legit test.
	TEST_F(circular_queue_test, CopyConstructor)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer1(16);
			EXPECT_EQ(16, buffer1.capacity());

			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			EXPECT_EQ(3, buffer1.size());

			// copy construct buffer2
			circular_queue<test_object> buffer2(buffer1);

			// reassign 'b' to 'd' to verify deep copy
			// this requires a construct/move assign/destroy
			buffer2.begin()[1] = test_object("d");

			EXPECT_STREQ(buffer2.begin()[1].getString().c_str(), "d");
			EXPECT_STREQ(buffer1.begin()[1].getString().c_str(), "b");
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(4, test_object::getconstructorcalls());
		EXPECT_EQ(7, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(3, test_object::getcopyconstructorcalls());
		EXPECT_EQ(1, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(2, test_object::getstringfunctioncalls());
	}

	// Tests that the move constructor moves stuff.
	TEST_F(circular_queue_test, MoveConstructor)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer1(16);
			EXPECT_EQ(16, buffer1.capacity());

			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			EXPECT_EQ(3, buffer1.size());

			// copy construct buffer2
			circular_queue<test_object> buffer2(std::move(buffer1));

			EXPECT_STREQ(buffer2.begin()[1].getString().c_str(), "b");
			EXPECT_EQ(0, buffer1.capacity());
			EXPECT_EQ(0, buffer1.size());
			EXPECT_EQ(16, buffer2.capacity());
			EXPECT_EQ(3, buffer2.size());
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(3, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(1, test_object::getstringfunctioncalls());
	}

	// Tests that the copy assignment makes a deep-copy of the other object. Test objects hold
	// strings so this is a legit test.
	TEST_F(circular_queue_test, CopyAssign)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer1(16);
			EXPECT_EQ(16, buffer1.capacity());

			// 3 constructor calls
			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			EXPECT_EQ(3, buffer1.size());

			// construct buffer2
			circular_queue<test_object> buffer2(4);
			EXPECT_EQ(4, buffer2.capacity());

			// 3 constructor calls
			buffer2.emplace_back('z');
			buffer2.emplace_back('y');
			buffer2.emplace_back('x');
			EXPECT_EQ(3, buffer2.size());

			// copy buffer 1 into buffer 2
			// 3 copy constructor calls
			buffer2 = buffer1;

			// reassign 'b' to 'd' to verify deep copy
			// this requires a construct/move assign/destroy
			EXPECT_STREQ(buffer2.begin()[1].getString().c_str(), "b");

			buffer2.begin()[1] = test_object("d");

			EXPECT_STREQ(buffer2.begin()[1].getString().c_str(), "d");
			EXPECT_STREQ(buffer1.begin()[1].getString().c_str(), "b");
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(7, test_object::getconstructorcalls());
		EXPECT_EQ(10, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(3, test_object::getcopyconstructorcalls());
		EXPECT_EQ(1, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(3, test_object::getstringfunctioncalls());
	}

	// Tests move assignment
	TEST_F(circular_queue_test, MoveAssign)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer1(16);
			EXPECT_EQ(16, buffer1.capacity());

			// 3 constructor calls
			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			EXPECT_EQ(3, buffer1.size());

			// construct buffer2
			circular_queue<test_object> buffer2(4);
			EXPECT_EQ(4, buffer2.capacity());

			// 3 constructor calls
			buffer2.emplace_back('z');
			buffer2.emplace_back('y');
			buffer2.emplace_back('x');
			EXPECT_EQ(3, buffer2.size());

			// copy buffer 1 into buffer 2
			// 3 copy constructor calls
			buffer2 = std::move(buffer1);

			EXPECT_STREQ(buffer2.begin()[1].getString().c_str(), "b");
			EXPECT_EQ(0, buffer1.capacity());
			EXPECT_EQ(0, buffer1.size());
			EXPECT_EQ(16, buffer2.capacity());
			EXPECT_EQ(3, buffer2.size());
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(6, test_object::getconstructorcalls());
		EXPECT_EQ(6, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(1, test_object::getstringfunctioncalls());
	}

	// Tests that the range constructor can build a queue based off of iterators from another container
	TEST_F(circular_queue_test, RangeAssign)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(4);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');

			circular_queue<test_object> buffer1(4);
			buffer.emplace_back('z');

			std::vector<test_object> vector1;
			vector1.reserve(3);
			vector1.emplace_back('e');
			vector1.emplace_back('f');
			vector1.emplace_back('g');

			// wrong type
			std::vector<double> vector2;
			vector2.push_back(1.4142135623730950488016887242097);
			vector2.push_back(3.1415926535897932384626433832795);

			// empty vector
			std::vector<test_object> vector3;

			// un-copyable vector
			std::vector<test_object> vector4;
			vector4.emplace_back('z', test_object::CopyConstructor);

			// normal range assign
			buffer1.assign(buffer.begin(), buffer.end());
			EXPECT_EQ(4, buffer.capacity());
			EXPECT_EQ(4, buffer.size());
			EXPECT_EQ(4, buffer1.capacity());
			EXPECT_EQ(4, buffer1.size());
			for (int i = 0; i < buffer.size(); i++)
			{
				EXPECT_STREQ(buffer[i].getString().c_str(), buffer1[i].getString().c_str());
			}

			// range assign from vector
			buffer1.assign(vector1.begin(), vector1.end());
			EXPECT_EQ(3, vector1.capacity());
			EXPECT_EQ(3, vector1.size());
			EXPECT_EQ(3, buffer1.capacity());
			EXPECT_EQ(3, buffer1.size());
			for (int i = 0; i < buffer1.size(); i++)
			{
				EXPECT_STREQ(vector1[i].getString().c_str(), buffer1[i].getString().c_str());
			}

			// fail on wrong type
			//			buffer1.assign(vector2.begin(), vector2.end());		// yup, this fails!

			// throw on capacity 0
			EXPECT_THROW(buffer1.assign(vector3.begin(), vector3.end()), std::length_error);

			// throw on capacity greater than max
			// -> untestable because I'd have to make another container with more than the max
			// capacity, which I can't.

			// throw if test_object constructor throws
			EXPECT_THROW(buffer1.assign(vector4.begin(), vector4.end()), std::bad_alloc);
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(9, test_object::getconstructorcalls());
		EXPECT_EQ(16, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(7, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(14, test_object::getstringfunctioncalls());
	}

	// Tests that the buffer is filled with the given value
	TEST_F(circular_queue_test, FillAssign)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			test_object fillValue('z');

			circular_queue<test_object> buffer(16);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			EXPECT_EQ(16, buffer.capacity());
			EXPECT_EQ(4, buffer.size());

			buffer.assign(8, fillValue);

			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(8, buffer.size());

			for (int i = 0; i < buffer.size(); i++)
			{
				EXPECT_EQ(buffer[i], fillValue);
			}

			// throw on capacity 0
			EXPECT_THROW(buffer.assign(0, fillValue), std::length_error);

			// throw on capacity greater than max
			EXPECT_THROW(buffer.assign(buffer.max_size() + 1, fillValue), std::bad_alloc);

			// throw if test_object constructor throws
			test_object failValue('a', test_object::CopyConstructor);
			EXPECT_THROW(buffer.assign(16, failValue), std::bad_alloc);
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(6, test_object::getconstructorcalls());
		EXPECT_EQ(14, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(8, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(0, test_object::getstringfunctioncalls());
	}

	// Tests that we can assign from an initializer_list
	TEST_F(circular_queue_test, InitializerListAssign)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(16);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			EXPECT_EQ(16, buffer.capacity());
			EXPECT_EQ(4, buffer.size());

			buffer = {'a', 'b', 'c', 'd', 'e'};
			EXPECT_STREQ(buffer[0].getString().c_str(), "a");
			EXPECT_STREQ(buffer[1].getString().c_str(), "b");
			EXPECT_STREQ(buffer[2].getString().c_str(), "c");
			EXPECT_STREQ(buffer[3].getString().c_str(), "d");
			EXPECT_STREQ(buffer[4].getString().c_str(), "e");
			EXPECT_EQ(5, buffer.capacity());
			EXPECT_EQ(5, buffer.size());

			buffer.assign({'x', 'y', 'z'});
			EXPECT_STREQ(buffer[0].getString().c_str(), "x");
			EXPECT_STREQ(buffer[1].getString().c_str(), "y");
			EXPECT_STREQ(buffer[2].getString().c_str(), "z");
			EXPECT_EQ(3, buffer.capacity());
			EXPECT_EQ(3, buffer.size());

			// throw on capacity 0
			EXPECT_THROW(buffer = {}, std::length_error);

			// throw on capacity greater than max
			// -> untestable because I'd have to write an initializer list with like a file full of
			// literal values.

			// throw if test_object constructor throws
			EXPECT_THROW(buffer = {test_object('z', test_object::CopyConstructor)}, std::bad_alloc);
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(13, test_object::getconstructorcalls());
		EXPECT_EQ(21, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(8, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(8, test_object::getstringfunctioncalls());
	}

	// Tests that element access gives the proper result for both const and non-const containers.
	TEST_F(circular_queue_test, ElementAccess)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(8);
			EXPECT_EQ(8, buffer.capacity());

			// 12 constructions
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');
			buffer.emplace_back('g');
			buffer.emplace_back('h');
			buffer.emplace_back('i');
			buffer.emplace_back('j');
			buffer.emplace_back('k');
			buffer.emplace_back('l');

			const circular_queue_test_wrapper constBuffer(buffer);
			EXPECT_EQ(8, constBuffer.capacity());
			EXPECT_EQ(8, constBuffer.size());

			// check constBuffer elements are const
			EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(constBuffer[0])>::type>::value);

			// check raw buffer data
			EXPECT_STREQ(buffer.data()[0].getString().c_str(), "i");
			EXPECT_STREQ(buffer.data()[1].getString().c_str(), "j");
			EXPECT_STREQ(buffer.data()[2].getString().c_str(), "k");
			EXPECT_STREQ(buffer.data()[3].getString().c_str(), "l");
			EXPECT_STREQ(buffer.data()[4].getString().c_str(), "e");
			EXPECT_STREQ(buffer.data()[5].getString().c_str(), "f");
			EXPECT_STREQ(buffer.data()[6].getString().c_str(), "g");
			EXPECT_STREQ(buffer.data()[7].getString().c_str(), "h");

			// check that data is what we expect ('e' to 'l'):
			char c = 'e';
			for (int i = 0; i < buffer.size(); ++i, ++c)
			{
				//operator[]
				EXPECT_STREQ(buffer[i].getString().c_str(), std::string(&c, 1).c_str());
				EXPECT_STREQ(constBuffer[i].getString().c_str(), std::string(&c, 1).c_str());

				// at()
				EXPECT_STREQ(buffer.at(i).getString().c_str(), std::string(&c, 1).c_str());
				EXPECT_STREQ(constBuffer.at(i).getString().c_str(), std::string(&c, 1).c_str());
			}

			// check range-based for
			std::vector<std::string> results = {"e", "f", "g", "h", "i", "j", "k", "l"};
			int                      index   = 0;
			for (const auto& i : buffer)
			{
				EXPECT_STREQ(i.getString().c_str(), results[index].c_str());
				++index;
			}

			index = 0;
			for (const auto& i : constBuffer)
			{
				EXPECT_STREQ(i.getString().c_str(), results[index].c_str());
				++index;
			}

			// front()
			EXPECT_STREQ(buffer.front().getString().c_str(), "e");
			EXPECT_STREQ(constBuffer.front().getString().c_str(), "e");

			// back()
			EXPECT_STREQ(buffer.back().getString().c_str(), "l");
			EXPECT_STREQ(constBuffer.back().getString().c_str(), "l");

			// check out_of_range exception
			EXPECT_NO_THROW(buffer.at(7));
			EXPECT_THROW(buffer.at(8), std::out_of_range);
			EXPECT_THROW(buffer.at(9), std::out_of_range);
			EXPECT_NO_THROW(constBuffer.at(7));
			EXPECT_THROW(constBuffer.at(8), std::out_of_range);
			EXPECT_THROW(constBuffer.at(9), std::out_of_range);

			// check out of range exception for unwrapped buffer
			circular_queue_test_wrapper buffer1(4);
			buffer1.emplace_back('x');
			buffer1.emplace_back('y');

			EXPECT_NO_THROW(buffer1.at(1));
			EXPECT_THROW(buffer1.at(2), std::out_of_range);
			EXPECT_THROW(buffer1.at(3), std::out_of_range);

			// check assigning to buffer doesn't change constBuffer.
			buffer[3] = 'z';    // construction/move assign/destruction
			EXPECT_STREQ(buffer[3].getString().c_str(), "z");
			EXPECT_STREQ(constBuffer[3].getString().c_str(), "h");
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(14, test_object::getconstructorcalls());
		EXPECT_EQ(22, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(8, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(1, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(62, test_object::getstringfunctioncalls());
	}

	// Tests resizing of the container
	TEST_F(circular_queue_test, Reserve)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer1(4);
			EXPECT_EQ(4, buffer1.capacity());

			circular_queue_test_wrapper buffer2(4);
			EXPECT_EQ(4, buffer2.capacity());

			// wrapped buffer
			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			buffer1.emplace_back('d');
			buffer1.emplace_back('e');
			buffer1.emplace_back('f');

			// unwrapped buffer
			buffer2.emplace_back('x');
			buffer2.emplace_back('y');
			buffer2.emplace_back('z');

			// do nothing if n < size()
			buffer1.reserve(2);
			buffer2.reserve(2);
			EXPECT_EQ(4, buffer1.capacity());
			EXPECT_EQ(4, buffer1.size());
			EXPECT_EQ(4, buffer2.capacity());
			EXPECT_EQ(3, buffer2.size());

			// throw if n > max_size
			EXPECT_THROW(buffer1.reserve(buffer1.max_size() + 1), std::bad_alloc);
			EXPECT_THROW(buffer2.reserve(buffer2.max_size() + 1), std::bad_alloc);
			EXPECT_EQ(4, buffer1.capacity());
			EXPECT_EQ(4, buffer1.size());
			EXPECT_EQ(4, buffer2.capacity());
			EXPECT_EQ(3, buffer2.size());

			// reserve valid values, check that iterators still work, data is correct
			buffer1.reserve(8);
			buffer2.reserve(8);

			EXPECT_EQ(8, buffer1.capacity());
			EXPECT_EQ(4, buffer1.size());
			EXPECT_EQ(8, buffer2.capacity());
			EXPECT_EQ(3, buffer2.size());

			EXPECT_STREQ(buffer1.data()[0].getString().c_str(), "c");
			EXPECT_STREQ(buffer1.data()[1].getString().c_str(), "d");
			EXPECT_STREQ(buffer1.data()[2].getString().c_str(), "e");
			EXPECT_STREQ(buffer1.data()[3].getString().c_str(), "f");

			EXPECT_STREQ(buffer2.data()[0].getString().c_str(), "x");
			EXPECT_STREQ(buffer2.data()[1].getString().c_str(), "y");
			EXPECT_STREQ(buffer2.data()[2].getString().c_str(), "z");

			char c = 'c';
			for (auto& itr : buffer1)
			{
				EXPECT_STREQ(itr.getString().c_str(), std::string(&c, 1).c_str());
				c++;
			}

			c = 'x';
			for (auto& itr : buffer2)
			{
				EXPECT_STREQ(itr.getString().c_str(), std::string(&c, 1).c_str());
				c++;
			}

			// test failure on move. The container can have side effects, but must still be in a valid
			// state. To me, that means size, capacity, and emplace work.
			circular_queue_test_wrapper buffer3(4);
			buffer3.emplace_back('a');
			buffer3.emplace_back('b', test_object::CopyConstructor | test_object::MoveConstructor);    // bad construction == no destructor call	// no move due to failure
			buffer3.emplace_back('c');                                                               // no move due to failure

			EXPECT_EQ(&buffer3.data()[0], buffer3.head());
			EXPECT_EQ(&buffer3.data()[3], buffer3.tail());
			EXPECT_EQ(buffer3.headParity(), buffer3.tailParity());

			EXPECT_THROW(buffer3.reserve(8), std::bad_alloc);

			EXPECT_EQ(1, buffer3.size());
			EXPECT_EQ(4, buffer3.capacity());
			EXPECT_EQ(&buffer3.data()[2], buffer3.head());
			EXPECT_EQ(&buffer3.data()[3], buffer3.tail());
			EXPECT_EQ(buffer3.headParity(), buffer3.tailParity());

			buffer3.emplace_back('d');

			EXPECT_EQ(2, buffer3.size());
			EXPECT_STREQ(buffer3[0].getString().c_str(), "c");
			EXPECT_STREQ(buffer3[1].getString().c_str(), "d");
			EXPECT_FALSE(buffer3.headParity() == buffer3.tailParity());
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(13, test_object::getconstructorcalls());
		EXPECT_EQ(20, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(8, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(16, test_object::getstringfunctioncalls());
	}

	// Tests the shrink_to_fit function
	TEST_F(circular_queue_test, ShrinkToFit)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// unwrapped buffer
			circular_queue_test_wrapper buffer(16);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');

			EXPECT_EQ(5, buffer.size());
			EXPECT_EQ(16, buffer.capacity());

			// check for shrunken capacity
			buffer.shrink_to_fit();
			EXPECT_EQ(5, buffer.size());
			EXPECT_EQ(5, buffer.capacity());

			// emplace and make sure it goes to the right place, and thus that the shrink occurred
			// properly
			buffer.emplace_back('f');

			char c;
			c = 'b';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(itr.getString().c_str(), std::string(&c, 1).c_str());
				c++;
			}
			EXPECT_EQ('f' + 1, c);

			EXPECT_STREQ(buffer.data()[0].getString().c_str(), "f");
			EXPECT_STREQ(buffer.data()[1].getString().c_str(), "b");
			EXPECT_STREQ(buffer.data()[2].getString().c_str(), "c");
			EXPECT_STREQ(buffer.data()[3].getString().c_str(), "d");
			EXPECT_STREQ(buffer.data()[4].getString().c_str(), "e");

			// wrapped buffer
			circular_queue_test_wrapper buffer1(8);
			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			buffer1.emplace_back('d');
			buffer1.emplace_back('e');
			buffer1.emplace_back('f');
			buffer1.emplace_back('g');
			buffer1.emplace_back('h');
			buffer1.emplace_back('i');
			buffer1.emplace_back('j');

			EXPECT_EQ(8, buffer1.size());
			EXPECT_EQ(8, buffer1.capacity());

			// pop some off the front to make things interesting
			for (int i = 0; i < 3; ++i)
			{
				buffer1.pop_front();
			}
			EXPECT_EQ(5, buffer1.size());
			EXPECT_EQ(8, buffer1.capacity());

			buffer1.shrink_to_fit();
			EXPECT_EQ(5, buffer1.size());
			EXPECT_EQ(5, buffer1.capacity());

			// emplace and make sure it goes to the right place, and thus that the shrink occurred
			// properly
			buffer1.emplace_back('k');

			c = 'g';
			for (auto& itr : buffer1)
			{
				EXPECT_STREQ(itr.getString().c_str(), std::string(&c, 1).c_str());
				c++;
			}
			EXPECT_EQ('k' + 1, c);

			EXPECT_STREQ(buffer1.data()[0].getString().c_str(), "k");
			EXPECT_STREQ(buffer1.data()[1].getString().c_str(), "g");
			EXPECT_STREQ(buffer1.data()[2].getString().c_str(), "h");
			EXPECT_STREQ(buffer1.data()[3].getString().c_str(), "i");
			EXPECT_STREQ(buffer1.data()[4].getString().c_str(), "j");

			// do nothing if we're already shrunk to size, including no reallocate
			test_object* data = buffer.data();
			buffer.shrink_to_fit();
			EXPECT_EQ(5, buffer.size());
			EXPECT_EQ(5, buffer.capacity());
			EXPECT_EQ(data, buffer.data());
		}

		EXPECT_EQ(17, test_object::getconstructorcalls());
		EXPECT_EQ(27, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(10, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(20, test_object::getstringfunctioncalls());
	}

	// Tests the resize function for a full, but unwrapped, buffer
	TEST_F(circular_queue_test, ResizeUnwrappedBuffer)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// UNWRAPPED BUFFER
			circular_queue_test_wrapper buffer(8);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');
			buffer.emplace_back('g');
			buffer.emplace_back('h');

			// do nothing if we're already shrunk to size, including no reallocate
			test_object* data = buffer.data();
			buffer.resize(8);    // since we don't specify a val, will default construct/destruct one
			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(data, buffer.data());

			// pad if we make it bigger
			buffer.resize(10, test_object('z'));
			EXPECT_EQ(10, buffer.size());
			EXPECT_EQ(10, buffer.capacity());

			EXPECT_STREQ(buffer[0].getString().c_str(), "a");
			EXPECT_STREQ(buffer[1].getString().c_str(), "b");
			EXPECT_STREQ(buffer[2].getString().c_str(), "c");
			EXPECT_STREQ(buffer[3].getString().c_str(), "d");
			EXPECT_STREQ(buffer[4].getString().c_str(), "e");
			EXPECT_STREQ(buffer[5].getString().c_str(), "f");
			EXPECT_STREQ(buffer[6].getString().c_str(), "g");
			EXPECT_STREQ(buffer[7].getString().c_str(), "h");
			EXPECT_STREQ(buffer[8].getString().c_str(), "z");
			EXPECT_STREQ(buffer[9].getString().c_str(), "z");

			// remove elements if we make it smaller
			buffer.resize(5);    // since we don't specify a val, will default construct/destruct one
			EXPECT_EQ(5, buffer.size());
			EXPECT_EQ(5, buffer.capacity());

			char c;
			c = 'a';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(itr.getString().c_str(), std::string(&c, 1).c_str());
				c++;
			}
			EXPECT_EQ('e' + 1, c);
		}

		EXPECT_EQ(9, test_object::getconstructorcalls());
		EXPECT_EQ(26, test_object::getdestructorcalls());
		EXPECT_EQ(2, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(13, test_object::getmoveconstructorcalls());
		EXPECT_EQ(2, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(15, test_object::getstringfunctioncalls());
	}

	// Tests the resize function for a wrapped and popped buffer
	TEST_F(circular_queue_test, ResizeWrappedBuffer)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// WRAPPED BUFFER
			circular_queue_test_wrapper buffer(8);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');
			buffer.emplace_back('g');
			buffer.emplace_back('h');
			buffer.emplace_back('i');
			buffer.emplace_back('j');
			buffer.emplace_back('k');
			buffer.emplace_back('l');

			// size == 4 after pops
			buffer.pop_front();
			buffer.pop_front();
			buffer.pop_back();
			buffer.pop_back();

			// do nothing if we're already shrunk to size, including no reallocate
			test_object* data = buffer.data();
			buffer.resize(4);
			EXPECT_EQ(4, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(data, buffer.data());

			// remove elements if we make it smaller
			buffer.resize(2);
			EXPECT_EQ(2, buffer.size());
			EXPECT_EQ(2, buffer.capacity());

			char c;
			c = 'g';

			// first elements should now be at the beginning of the buffer
			EXPECT_STREQ(std::string(&c, 1).c_str(), buffer.data()[0].getString().c_str());

			// iterators should point to first two elements
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(itr.getString().c_str(), std::string(&c, 1).c_str());
				c++;
			}
			EXPECT_EQ('h' + 1, c);

			// pad if we make it bigger
			buffer.resize(10, test_object('z'));
			EXPECT_EQ(10, buffer.size());
			EXPECT_EQ(10, buffer.capacity());

			EXPECT_STREQ(buffer[0].getString().c_str(), "g");
			EXPECT_STREQ(buffer[1].getString().c_str(), "h");
			EXPECT_STREQ(buffer[2].getString().c_str(), "z");
			EXPECT_STREQ(buffer[3].getString().c_str(), "z");
			EXPECT_STREQ(buffer[4].getString().c_str(), "z");
			EXPECT_STREQ(buffer[5].getString().c_str(), "z");
			EXPECT_STREQ(buffer[6].getString().c_str(), "z");
			EXPECT_STREQ(buffer[7].getString().c_str(), "z");
			EXPECT_STREQ(buffer[8].getString().c_str(), "z");
			EXPECT_STREQ(buffer[9].getString().c_str(), "z");
		}

		EXPECT_EQ(13, test_object::getconstructorcalls());
		EXPECT_EQ(27, test_object::getdestructorcalls());
		EXPECT_EQ(2, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(4, test_object::getmoveconstructorcalls());
		EXPECT_EQ(8, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(13, test_object::getstringfunctioncalls());
	}

	// Tests the clear function
	TEST_F(circular_queue_test, Clear)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(8);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');
			buffer.emplace_back('g');
			buffer.emplace_back('h');
			buffer.emplace_back('i');
			buffer.emplace_back('j');
			buffer.emplace_back('k');
			buffer.emplace_back('l');

			EXPECT_EQ(12, test_object::getconstructorcalls());

			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(4, test_object::getdestructorcalls());

			buffer.clear();

			EXPECT_EQ(8, test_object::getdestructorcalls());

			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
		}
	}

	// Tests the push_back function for lvalues and rvalues.
	TEST_F(circular_queue_test, PushBack)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// check exception cases first to see if they screw up the container state (hint: they shouldn't)
			test_object failValue('z', test_object::CopyConstructor | test_object::MoveConstructor);
			test_object lvalue('a');
			char       c;
			EXPECT_EQ(2, test_object::getconstructorcalls());

			circular_queue_test_wrapper buffer(8);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// lvalue exception
			EXPECT_THROW(buffer.push_back(failValue), std::bad_alloc);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(buffer.head(), buffer.data());
			EXPECT_EQ(buffer.tail(), buffer.data());

			// rvalue exception
			EXPECT_THROW(buffer.push_back(std::move(failValue)), std::bad_alloc);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(buffer.head(), buffer.data());
			EXPECT_EQ(buffer.tail(), buffer.data());

			// wrap buffer 2.5x with lvalues
			for (int i = 0; i < 20; i++)
			{
				buffer.push_back(lvalue);
				lvalue = (char) (lvalue.getString().c_str()[0] + 1);
			}

			// correct size
			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// correct data
			EXPECT_STREQ("q", buffer.data()[0].getString().c_str());

			c = 'm';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(std::string(&c, 1).c_str(), itr.getString().c_str());
				c++;
			}
			EXPECT_EQ('t' + 1, c);

			// expected constructor/destructor/assignment (temp objects)
			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());
			EXPECT_EQ(32, test_object::getcopyassignmentcalls());    // updating lvalue + last 12 contained members
			EXPECT_EQ(8, test_object::getcopyconstructorcalls());    // first 8 contained members

			// CLEAR //
			buffer.clear();
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(8, test_object::getdestructorcalls());

			// wrap buffer 2.5x with rvalues
			buffer.push_back(test_object('a'));
			buffer.push_back(test_object('b'));
			buffer.push_back(test_object('c'));
			buffer.push_back(test_object('d'));
			buffer.push_back(test_object('e'));
			buffer.push_back(test_object('f'));
			buffer.push_back(test_object('g'));
			buffer.push_back(test_object('h'));
			buffer.push_back(test_object('i'));
			buffer.push_back(test_object('j'));
			buffer.push_back(test_object('k'));
			buffer.push_back(test_object('l'));
			buffer.push_back(test_object('m'));
			buffer.push_back(test_object('n'));
			buffer.push_back(test_object('o'));
			buffer.push_back(test_object('p'));
			buffer.push_back(test_object('q'));
			buffer.push_back(test_object('r'));
			buffer.push_back(test_object('s'));
			buffer.push_back(test_object('t'));

			// correct size
			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// correct data
			EXPECT_STREQ("q", buffer.data()[0].getString().c_str());

			c = 'm';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(std::string(&c, 1).c_str(), itr.getString().c_str());
				c++;
			}
			EXPECT_EQ('t' + 1, c);

			EXPECT_EQ(20, test_object::getconstructorcalls());    // temp objects
			EXPECT_EQ(20, test_object::getdestructorcalls());     // temp objects
			EXPECT_EQ(8, test_object::getmoveconstructorcalls());
			EXPECT_EQ(12, test_object::getmoveassignmentcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(10, test_object::getdestructorcalls());    // rvalue container and original two test objects. lvalue container already cleared.
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(38, test_object::getstringfunctioncalls());
	}

	// Tests the push_front function for lvalues and rvalues.
	TEST_F(circular_queue_test, PushFront)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// check exception cases first to see if they screw up the container state (hint: they shouldn't)
			test_object failValue('z', test_object::CopyConstructor | test_object::MoveConstructor);
			test_object lvalue('t');
			char       c;
			EXPECT_EQ(2, test_object::getconstructorcalls());

			circular_queue_test_wrapper buffer(8);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// lvalue exception
			EXPECT_THROW(buffer.push_front(failValue), std::bad_alloc);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(buffer.head(), buffer.data());
			EXPECT_EQ(buffer.tail(), buffer.data());

			// rvalue exception
			EXPECT_THROW(buffer.push_front(std::move(failValue)), std::bad_alloc);
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(buffer.head(), buffer.data());
			EXPECT_EQ(buffer.tail(), buffer.data());

			// wrap buffer 2.5x with lvalues
			for (int i = 0; i < 20; i++)
			{
				buffer.push_front(lvalue);
				lvalue = (char) (lvalue.getString().c_str()[0] - 1);
			}

			// correct size
			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// correct data
			EXPECT_STREQ("e", buffer.data()[0].getString().c_str());

			c = 'a';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(std::string(&c, 1).c_str(), itr.getString().c_str());
				c++;
			}
			EXPECT_EQ('h' + 1, c);

			// expected constructor/destructor/assignment (temp objects)
			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());
			EXPECT_EQ(32, test_object::getcopyassignmentcalls());    // updating lvalue + last 12 contained members
			EXPECT_EQ(8, test_object::getcopyconstructorcalls());    // first 8 contained members

			// CLEAR //
			buffer.clear();
			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(8, test_object::getdestructorcalls());

			// wrap buffer 2.5x with rvalues
			buffer.push_front(test_object('t'));
			buffer.push_front(test_object('s'));
			buffer.push_front(test_object('r'));
			buffer.push_front(test_object('q'));
			buffer.push_front(test_object('p'));
			buffer.push_front(test_object('o'));
			buffer.push_front(test_object('n'));
			buffer.push_front(test_object('m'));
			buffer.push_front(test_object('l'));
			buffer.push_front(test_object('k'));
			buffer.push_front(test_object('j'));
			buffer.push_front(test_object('i'));
			buffer.push_front(test_object('h'));
			buffer.push_front(test_object('g'));
			buffer.push_front(test_object('f'));
			buffer.push_front(test_object('e'));
			buffer.push_front(test_object('d'));
			buffer.push_front(test_object('c'));
			buffer.push_front(test_object('b'));
			buffer.push_front(test_object('a'));

			// correct size
			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			// correct data
			EXPECT_STREQ("e", buffer.data()[0].getString().c_str());

			c = 'a';
			for (auto& itr : buffer)
			{
				EXPECT_STREQ(std::string(&c, 1).c_str(), itr.getString().c_str());
				c++;
			}
			EXPECT_EQ('h' + 1, c);

			EXPECT_EQ(20, test_object::getconstructorcalls());    // temp objects
			EXPECT_EQ(20, test_object::getdestructorcalls());     // temp objects
			EXPECT_EQ(8, test_object::getmoveconstructorcalls());
			EXPECT_EQ(12, test_object::getmoveassignmentcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(10, test_object::getdestructorcalls());    // rvalue container and original two test objects. lvalue container already cleared.
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(38, test_object::getstringfunctioncalls());
	}

	// Tests the emplace_back function for lvalues and rvalues.
	TEST_F(circular_queue_test, Emplace)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// create test objects
			test_object                           c('c');
			test_object                           d('d');
			test_object                           emplaceValue('z');
			test_object                           failValue('z', test_object::CopyConstructor);
			circular_queue<test_object>::iterator result;

			circular_queue_test_wrapper empty(8);

			circular_queue_test_wrapper full(8);
			full.emplace_back('a');
			full.emplace_back('b');
			full.emplace_back('c');
			full.emplace_back('d');
			full.emplace_back('e');
			full.emplace_back('f');
			full.emplace_back('g');
			full.emplace_back('h');

			circular_queue_test_wrapper partial(8);
			partial.emplace_back('a');
			partial.emplace_back('b');
			partial.emplace_back('c');
			partial.emplace_back('d');

			EXPECT_EQ(16, test_object::getconstructorcalls());

			// put bad value into empty container
			EXPECT_THROW(empty.emplace(empty.begin(), failValue), std::bad_alloc);

			// expect no state changes
			EXPECT_EQ(0, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_EQ(empty.head(), empty.data());
			EXPECT_EQ(empty.tail(), empty.data());

			// construct into empty container
			result = empty.emplace(empty.begin(), 'z');
			EXPECT_EQ(1, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_TRUE(result == empty.begin());
			EXPECT_STREQ("z", empty.begin()->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create 'z'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// clear
			empty.clear();
			EXPECT_EQ(1, test_object::getdestructorcalls());

			// put rvalue into empty container
			empty.emplace(empty.begin(), 'x');
			EXPECT_EQ(1, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_TRUE(result == empty.begin());
			EXPECT_STREQ("x", empty.begin()->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create 'x'
			EXPECT_EQ(0, test_object::getdestructorcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// emplace at begin, middle, end of partial
			result = partial.emplace(partial.begin() + 2, 'z');

			EXPECT_EQ(result, std::prev(std::find(partial.begin(), partial.end(), c)));
			EXPECT_EQ(emplaceValue, *result);

			EXPECT_STREQ("a", partial[0].getString().c_str());
			EXPECT_STREQ("b", partial[1].getString().c_str());
			EXPECT_STREQ("z", partial[2].getString().c_str());
			EXPECT_STREQ("c", partial[3].getString().c_str());
			EXPECT_STREQ("d", partial[4].getString().c_str());

			EXPECT_EQ(5, partial.size());
			EXPECT_EQ(8, partial.capacity());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create 'z'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(2, test_object::getmoveconstructorcalls());
			EXPECT_EQ(4, test_object::getmoveassignmentcalls());
			EXPECT_EQ(2, test_object::getdestructorcalls());

			result = partial.emplace(partial.begin(), 'z');

			EXPECT_TRUE(result == partial.begin());
			EXPECT_EQ(emplaceValue, *result);

			EXPECT_STREQ("z", partial[0].getString().c_str());
			EXPECT_STREQ("a", partial[1].getString().c_str());
			EXPECT_STREQ("b", partial[2].getString().c_str());
			EXPECT_STREQ("z", partial[3].getString().c_str());
			EXPECT_STREQ("c", partial[4].getString().c_str());
			EXPECT_STREQ("d", partial[5].getString().c_str());

			EXPECT_EQ(6, partial.size());
			EXPECT_EQ(8, partial.capacity());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create 'z'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(5, test_object::getmoveconstructorcalls());
			EXPECT_EQ(10, test_object::getmoveassignmentcalls());
			EXPECT_EQ(5, test_object::getdestructorcalls());

			result = partial.emplace(partial.end(), 'z');

			EXPECT_TRUE(result == std::prev(partial.end()));
			EXPECT_EQ(emplaceValue, *result);

			EXPECT_EQ(7, partial.size());
			EXPECT_EQ(8, partial.capacity());

			EXPECT_STREQ("z", partial[0].getString().c_str());
			EXPECT_STREQ("a", partial[1].getString().c_str());
			EXPECT_STREQ("b", partial[2].getString().c_str());
			EXPECT_STREQ("z", partial[3].getString().c_str());
			EXPECT_STREQ("c", partial[4].getString().c_str());
			EXPECT_STREQ("d", partial[5].getString().c_str());
			EXPECT_STREQ("z", partial[6].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create 'z'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// emplace into middle of full container
			circular_queue_test_wrapper::iterator dPos = std::find(full.begin(), full.end(), d);
			EXPECT_STREQ("d", dPos->getString().c_str());

			result = full.emplace(dPos, 'z');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_EQ(std::prev(std::find(full.begin(), full.end(), d)), result);

			EXPECT_STREQ("b", full[0].getString().c_str());
			EXPECT_STREQ("c", full[1].getString().c_str());
			EXPECT_STREQ("z", full[2].getString().c_str());
			EXPECT_STREQ("d", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create 'z'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(5, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(10, test_object::getmoveassignmentcalls());    // swap temporaries + elements moved
			EXPECT_EQ(6, test_object::getdestructorcalls());         // swap temporaries + element replaced

			// emplace into middle of full container
			result = full.emplace(full.begin() + 4, 'x');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_STREQ("c", full[0].getString().c_str());
			EXPECT_STREQ("z", full[1].getString().c_str());
			EXPECT_STREQ("d", full[2].getString().c_str());
			EXPECT_STREQ("x", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // temp rvalue
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(4, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(8, test_object::getmoveassignmentcalls());     // swap temporaries
			EXPECT_EQ(5, test_object::getdestructorcalls());         // swap temporaries + element replaced

			// emplace at beginning of full
			result = full.emplace(full.begin(), 'x');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_TRUE(result == full.begin());
			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_STREQ("x", full[0].getString().c_str());
			EXPECT_STREQ("z", full[1].getString().c_str());
			EXPECT_STREQ("d", full[2].getString().c_str());
			EXPECT_STREQ("x", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // temp rvalue
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(7, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(14, test_object::getmoveassignmentcalls());    // swap temporaries
			EXPECT_EQ(8, test_object::getdestructorcalls());         // swap temporaries + element replaced

			// emplace at end of full
			result = full.emplace(full.end(), 'y');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_EQ(result, std::prev(full.end()));
			EXPECT_STREQ("y", result->getString().c_str());

			EXPECT_STREQ("z", full[0].getString().c_str());
			EXPECT_STREQ("d", full[1].getString().c_str());
			EXPECT_STREQ("x", full[2].getString().c_str());
			EXPECT_STREQ("e", full[3].getString().c_str());
			EXPECT_STREQ("f", full[4].getString().c_str());
			EXPECT_STREQ("g", full[5].getString().c_str());
			EXPECT_STREQ("h", full[6].getString().c_str());
			EXPECT_STREQ("y", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // element emplaced
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(1, test_object::getdestructorcalls());    // element removed
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(20, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(56, test_object::getstringfunctioncalls());
	}

	// Tests insertion of a single element
	TEST_F(circular_queue_test, InsertSingle)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// create test objects
			test_object                           c('c');
			test_object                           d('d');
			test_object                           insertValue('z');
			test_object                           failValue('z', test_object::CopyConstructor);
			circular_queue<test_object>::iterator result;

			circular_queue_test_wrapper empty(8);

			circular_queue_test_wrapper full(8);
			full.emplace_back('a');
			full.emplace_back('b');
			full.emplace_back('c');
			full.emplace_back('d');
			full.emplace_back('e');
			full.emplace_back('f');
			full.emplace_back('g');
			full.emplace_back('h');

			circular_queue_test_wrapper partial(8);
			partial.emplace_back('a');
			partial.emplace_back('b');
			partial.emplace_back('c');
			partial.emplace_back('d');

			EXPECT_EQ(16, test_object::getconstructorcalls());

			// put bad value into empty container
			EXPECT_THROW(empty.insert(empty.begin(), failValue), std::bad_alloc);

			// expect no state changes
			EXPECT_EQ(0, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_EQ(empty.head(), empty.data());
			EXPECT_EQ(empty.tail(), empty.data());

			// put lvalue into empty container
			result = empty.insert(empty.begin(), insertValue);
			EXPECT_EQ(1, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_TRUE(result == empty.begin());
			EXPECT_STREQ("z", empty.begin()->getString().c_str());

			EXPECT_EQ(1, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// clear
			empty.clear();
			EXPECT_EQ(1, test_object::getdestructorcalls());

			// put rvalue into empty container
			empty.insert(empty.begin(), 'x');
			EXPECT_EQ(1, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_TRUE(result == empty.begin());
			EXPECT_STREQ("x", empty.begin()->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(1, test_object::getdestructorcalls());     // destroy temp 'x'
			EXPECT_EQ(1, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// insert at begin, middle, end of partial
			result = partial.insert(partial.begin() + 2, insertValue);

			EXPECT_EQ(result, std::prev(std::find(partial.begin(), partial.end(), c)));
			EXPECT_EQ(insertValue, *result);

			EXPECT_STREQ("a", partial[0].getString().c_str());
			EXPECT_STREQ("b", partial[1].getString().c_str());
			EXPECT_STREQ("z", partial[2].getString().c_str());
			EXPECT_STREQ("c", partial[3].getString().c_str());
			EXPECT_STREQ("d", partial[4].getString().c_str());

			EXPECT_EQ(5, partial.size());
			EXPECT_EQ(8, partial.capacity());

			EXPECT_EQ(1, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(2, test_object::getmoveconstructorcalls());
			EXPECT_EQ(4, test_object::getmoveassignmentcalls());
			EXPECT_EQ(2, test_object::getdestructorcalls());

			result = partial.insert(partial.begin(), insertValue);

			EXPECT_TRUE(result == partial.begin());
			EXPECT_EQ(insertValue, *result);

			EXPECT_STREQ("z", partial[0].getString().c_str());
			EXPECT_STREQ("a", partial[1].getString().c_str());
			EXPECT_STREQ("b", partial[2].getString().c_str());
			EXPECT_STREQ("z", partial[3].getString().c_str());
			EXPECT_STREQ("c", partial[4].getString().c_str());
			EXPECT_STREQ("d", partial[5].getString().c_str());

			EXPECT_EQ(6, partial.size());
			EXPECT_EQ(8, partial.capacity());

			EXPECT_EQ(1, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(5, test_object::getmoveconstructorcalls());
			EXPECT_EQ(10, test_object::getmoveassignmentcalls());
			EXPECT_EQ(5, test_object::getdestructorcalls());

			result = partial.insert(partial.end(), insertValue);

			EXPECT_TRUE(result == std::prev(partial.end()));
			EXPECT_EQ(insertValue, *result);

			EXPECT_EQ(7, partial.size());
			EXPECT_EQ(8, partial.capacity());

			EXPECT_STREQ("z", partial[0].getString().c_str());
			EXPECT_STREQ("a", partial[1].getString().c_str());
			EXPECT_STREQ("b", partial[2].getString().c_str());
			EXPECT_STREQ("z", partial[3].getString().c_str());
			EXPECT_STREQ("c", partial[4].getString().c_str());
			EXPECT_STREQ("d", partial[5].getString().c_str());
			EXPECT_STREQ("z", partial[6].getString().c_str());

			EXPECT_EQ(1, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// put lvalue into middle of full container
			circular_queue_test_wrapper::iterator dPos = std::find(full.begin(), full.end(), d);
			EXPECT_STREQ("d", dPos->getString().c_str());

			result = full.insert(dPos, insertValue);
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_EQ(std::prev(std::find(full.begin(), full.end(), d)), result);

			EXPECT_STREQ("b", full[0].getString().c_str());
			EXPECT_STREQ("c", full[1].getString().c_str());
			EXPECT_STREQ("z", full[2].getString().c_str());
			EXPECT_STREQ("d", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(1, test_object::getcopyassignmentcalls());
			EXPECT_EQ(5, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(10, test_object::getmoveassignmentcalls());    // swap temporaries + elements moved
			EXPECT_EQ(5, test_object::getdestructorcalls());         // swap temporaries

			// insert rvalue into middle of full container
			result = full.insert(full.begin() + 4, 'x');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_STREQ("c", full[0].getString().c_str());
			EXPECT_STREQ("z", full[1].getString().c_str());
			EXPECT_STREQ("d", full[2].getString().c_str());
			EXPECT_STREQ("x", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // temp rvalue
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(4, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(9, test_object::getmoveassignmentcalls());     // swap temporaries + elements moved
			EXPECT_EQ(5, test_object::getdestructorcalls());

			// insert at beginning of full
			result = full.insert(full.begin(), 'x');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_TRUE(result == full.begin());
			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_STREQ("x", full[0].getString().c_str());
			EXPECT_STREQ("z", full[1].getString().c_str());
			EXPECT_STREQ("d", full[2].getString().c_str());
			EXPECT_STREQ("x", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // temp rvalue
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(7, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(15, test_object::getmoveassignmentcalls());    // swap temporaries + elements moved + rvalue input move
			EXPECT_EQ(8, test_object::getdestructorcalls());

			// insert at end of full
			result = full.insert(full.end(), 'y');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_EQ(result, std::prev(full.end()));
			EXPECT_STREQ("y", result->getString().c_str());

			EXPECT_STREQ("z", full[0].getString().c_str());
			EXPECT_STREQ("d", full[1].getString().c_str());
			EXPECT_STREQ("x", full[2].getString().c_str());
			EXPECT_STREQ("e", full[3].getString().c_str());
			EXPECT_STREQ("f", full[4].getString().c_str());
			EXPECT_STREQ("g", full[5].getString().c_str());
			EXPECT_STREQ("h", full[6].getString().c_str());
			EXPECT_STREQ("y", full[7].getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // temp rvalue
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(1, test_object::getmoveassignmentcalls());    // elements inserted
			EXPECT_EQ(1, test_object::getdestructorcalls());        // temp rvalue
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(20, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(56, test_object::getstringfunctioncalls());
	}

	// Tests insertion of a single element
	TEST_F(circular_queue_test, InsertFill)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// create test objects
			test_object d('d');
			test_object f('f');
			test_object insertValue('z');
			test_object failValue('z', test_object::CopyConstructor);

			circular_queue_test_wrapper::iterator result;

			circular_queue_test_wrapper empty(8);

			circular_queue_test_wrapper full(8);
			full.emplace_back('a');
			full.emplace_back('b');
			full.emplace_back('c');
			full.emplace_back('d');
			full.emplace_back('e');
			full.emplace_back('f');
			full.emplace_back('g');
			full.emplace_back('h');

			circular_queue_test_wrapper partial(16);
			partial.emplace_back('a');
			partial.emplace_back('b');
			partial.emplace_back('c');
			partial.emplace_back('d');

			EXPECT_EQ(16, test_object::getconstructorcalls());

			// put bad value into empty container
			EXPECT_THROW(empty.insert(empty.begin(), 3, failValue), std::bad_alloc);

			// expect no state changes
			EXPECT_EQ(0, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_EQ(empty.head(), empty.data());
			EXPECT_EQ(empty.tail(), empty.data());

			test_object::clearfunctioncalls();

			// put lvalue into empty container
			result = empty.insert(empty.begin(), 3, insertValue);
			EXPECT_EQ(3, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_STREQ("z", empty.begin()[0].getString().c_str());
			EXPECT_STREQ("z", empty.begin()[1].getString().c_str());
			EXPECT_STREQ("z", empty.begin()[2].getString().c_str());

			EXPECT_EQ(result, empty.begin());
			EXPECT_STREQ("z", result->getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(3, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// clear
			empty.clear();
			EXPECT_EQ(3, test_object::getdestructorcalls());

			// put rvalue into empty container
			result = empty.insert(empty.end(), 3, 'x');
			EXPECT_EQ(3, empty.size());
			EXPECT_EQ(8, empty.capacity());
			EXPECT_STREQ("x", empty.begin()[0].getString().c_str());
			EXPECT_STREQ("x", empty.begin()[1].getString().c_str());
			EXPECT_STREQ("x", empty.begin()[2].getString().c_str());

			EXPECT_EQ(result, empty.end() - 3);
			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(3, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(1, test_object::getdestructorcalls());

			// insert at begin, middle, end of partial
			result = partial.insert(partial.begin() + 2, 3, insertValue);

			EXPECT_STREQ("a", partial[0].getString().c_str());
			EXPECT_STREQ("b", partial[1].getString().c_str());
			EXPECT_STREQ("z", partial[2].getString().c_str());
			EXPECT_STREQ("z", partial[3].getString().c_str());
			EXPECT_STREQ("z", partial[4].getString().c_str());
			EXPECT_STREQ("c", partial[5].getString().c_str());
			EXPECT_STREQ("d", partial[6].getString().c_str());

			EXPECT_EQ(7, partial.size());
			EXPECT_EQ(16, partial.capacity());

			EXPECT_EQ(partial.begin() + 2, result);
			EXPECT_STREQ("z", result->getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(3, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(3 * (5 - 1), test_object::getmoveconstructorcalls());
			EXPECT_EQ(3 * (5 - 1) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(3 * (5 - 1), test_object::getdestructorcalls());

			result = partial.insert(partial.begin(), 2, 'x');

			EXPECT_STREQ("x", partial[0].getString().c_str());
			EXPECT_STREQ("x", partial[1].getString().c_str());
			EXPECT_STREQ("a", partial[2].getString().c_str());
			EXPECT_STREQ("b", partial[3].getString().c_str());
			EXPECT_STREQ("z", partial[4].getString().c_str());
			EXPECT_STREQ("z", partial[5].getString().c_str());
			EXPECT_STREQ("z", partial[6].getString().c_str());
			EXPECT_STREQ("c", partial[7].getString().c_str());
			EXPECT_STREQ("d", partial[8].getString().c_str());

			EXPECT_EQ(9, partial.size());
			EXPECT_EQ(16, partial.capacity());

			EXPECT_EQ(partial.begin(), result);
			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(2, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(2 * (9 - 1), test_object::getmoveconstructorcalls());       // create swap temporary: numInserted*(dist from insertion point -1)
			EXPECT_EQ(2 * (9 - 1) * 2, test_object::getmoveassignmentcalls());    // swap assignments: 2 per swap temporary
			EXPECT_EQ(2 * (9 - 1) + 1, test_object::getdestructorcalls());        // swap creations + normal creations

			result = partial.insert(partial.end(), 4, insertValue);

			EXPECT_EQ(13, partial.size());
			EXPECT_EQ(16, partial.capacity());

			EXPECT_STREQ("x", partial[0].getString().c_str());
			EXPECT_STREQ("x", partial[1].getString().c_str());
			EXPECT_STREQ("a", partial[2].getString().c_str());
			EXPECT_STREQ("b", partial[3].getString().c_str());
			EXPECT_STREQ("z", partial[4].getString().c_str());
			EXPECT_STREQ("z", partial[5].getString().c_str());
			EXPECT_STREQ("z", partial[6].getString().c_str());
			EXPECT_STREQ("c", partial[7].getString().c_str());
			EXPECT_STREQ("d", partial[8].getString().c_str());
			EXPECT_STREQ("z", partial[9].getString().c_str());
			EXPECT_STREQ("z", partial[10].getString().c_str());
			EXPECT_STREQ("z", partial[11].getString().c_str());
			EXPECT_STREQ("z", partial[12].getString().c_str());

			EXPECT_EQ(partial.end() - 4, result);
			EXPECT_STREQ("z", result->getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(4, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // no swaps needed when inserting at end
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// put lvalue into middle of full container
			result = full.insert(full.begin() + 5, 2, insertValue);
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("c", full[0].getString().c_str());
			EXPECT_STREQ("d", full[1].getString().c_str());
			EXPECT_STREQ("e", full[2].getString().c_str());
			EXPECT_STREQ("z", full[3].getString().c_str());
			EXPECT_STREQ("z", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(std::find(full.begin(), full.end(), f) - 2, result);
			EXPECT_STREQ("z", result->getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(2, test_object::getcopyassignmentcalls());                  // since full, we assign rather than construct
			EXPECT_EQ(2 * (5 - 1), test_object::getmoveconstructorcalls());       // create swap temporary: numInserted*(dist from insertion point -1)
			EXPECT_EQ(2 * (5 - 1) * 2, test_object::getmoveassignmentcalls());    // swap assignments: 2 per swap temporary
			EXPECT_EQ(2 * (5 - 1) + 0, test_object::getdestructorcalls());        // swap creations + normal creations

			// put rvalue into middle of full container
			result = full.insert(full.begin() + 5, 2, 'x');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("e", full[0].getString().c_str());
			EXPECT_STREQ("z", full[1].getString().c_str());
			EXPECT_STREQ("z", full[2].getString().c_str());
			EXPECT_STREQ("x", full[3].getString().c_str());
			EXPECT_STREQ("x", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(std::find(full.begin(), full.end(), f) - 2, result);
			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(2, test_object::getcopyassignmentcalls());                  // since full, we assign rather than construct
			EXPECT_EQ(2 * (5 - 1), test_object::getmoveconstructorcalls());       // create swap temporary: numInserted*(dist from insertion point -1)
			EXPECT_EQ(2 * (5 - 1) * 2, test_object::getmoveassignmentcalls());    // swap assignments: 2 per swap temporary
			EXPECT_EQ(2 * (5 - 1) + 1, test_object::getdestructorcalls());        // swap creations + normal creations

			// put rvalue into beginning of full container
			result = full.insert(full.begin(), 4, 'y');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("y", full[0].getString().c_str());
			EXPECT_STREQ("y", full[1].getString().c_str());
			EXPECT_STREQ("y", full[2].getString().c_str());
			EXPECT_STREQ("y", full[3].getString().c_str());
			EXPECT_STREQ("x", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(full.begin(), result);
			EXPECT_STREQ("y", result->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(4, test_object::getcopyassignmentcalls());                  // since full, we assign rather than construct
			EXPECT_EQ(4 * (8 - 1), test_object::getmoveconstructorcalls());       // create swap temporary: numInserted*(dist from insertion point -1)
			EXPECT_EQ(4 * (8 - 1) * 2, test_object::getmoveassignmentcalls());    // swap assignments: 2 per swap temporary
			EXPECT_EQ(4 * (8 - 1) + 1, test_object::getdestructorcalls());        // swap creations + normal creations

			// put rvalue into end of full container
			result = full.insert(full.end(), 5, 'q');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("f", full[0].getString().c_str());
			EXPECT_STREQ("g", full[1].getString().c_str());
			EXPECT_STREQ("h", full[2].getString().c_str());
			EXPECT_STREQ("q", full[3].getString().c_str());
			EXPECT_STREQ("q", full[4].getString().c_str());
			EXPECT_STREQ("q", full[5].getString().c_str());
			EXPECT_STREQ("q", full[6].getString().c_str());
			EXPECT_STREQ("q", full[7].getString().c_str());

			EXPECT_EQ(full.end() - 5, result);
			EXPECT_STREQ("q", result->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(5, test_object::getcopyassignmentcalls());     // pushing two new values onto queue
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());     // swap temporaries + elements moved
			EXPECT_EQ(1, test_object::getdestructorcalls());         // destroy temp 'x' + swap temporaries

			// put size n > capacity
			result = full.insert(full.begin() + 4, 25, 'a');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("a", full[0].getString().c_str());
			EXPECT_STREQ("a", full[1].getString().c_str());
			EXPECT_STREQ("a", full[2].getString().c_str());
			EXPECT_STREQ("a", full[3].getString().c_str());
			EXPECT_STREQ("a", full[4].getString().c_str());
			EXPECT_STREQ("a", full[5].getString().c_str());
			EXPECT_STREQ("a", full[6].getString().c_str());
			EXPECT_STREQ("a", full[7].getString().c_str());

			EXPECT_EQ(full.begin(), result);
			EXPECT_STREQ("a", result->getString().c_str());

			EXPECT_EQ(1, test_object::getconstructorcalls());    // create temp 'x'
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(8, test_object::getcopyassignmentcalls());     // pushing two new values onto queue
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // swap temporaries
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());     // swap temporaries + elements moved
			EXPECT_EQ(1, test_object::getdestructorcalls());         // destroy temp 'x' + swap temporaries

			// insert 0
			full.emplace_back('a');
			full.emplace_back('b');
			full.emplace_back('c');
			full.emplace_back('d');
			full.emplace_back('e');
			full.emplace_back('f');
			full.emplace_back('g');
			full.emplace_back('h');

			result = full.insert(full.begin() + 4, 0, 'z');
			EXPECT_EQ(8, full.size());
			EXPECT_EQ(8, full.capacity());

			EXPECT_STREQ("a", full[0].getString().c_str());
			EXPECT_STREQ("b", full[1].getString().c_str());
			EXPECT_STREQ("c", full[2].getString().c_str());
			EXPECT_STREQ("d", full[3].getString().c_str());
			EXPECT_STREQ("e", full[4].getString().c_str());
			EXPECT_STREQ("f", full[5].getString().c_str());
			EXPECT_STREQ("g", full[6].getString().c_str());
			EXPECT_STREQ("h", full[7].getString().c_str());

			EXPECT_EQ(9, test_object::getconstructorcalls());    // create temp 'z', emplace temporaries
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // no swaps because nothing inserted
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(9, test_object::getdestructorcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(28, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(93, test_object::getstringfunctioncalls());
	}

	// This test checks inserting ranges from other containers
	TEST_F(circular_queue_test, InsertRange)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper           buffer(8);
			circular_queue_test_wrapper::iterator result;

			const std::deque<test_object> deq = {'x', 'y', 'z'};

			const std::vector<test_object> big = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l'};

			std::vector<test_object> vec;
			vec.emplace_back('a');
			vec.emplace_back('b');
			vec.emplace_back('c');
			vec.emplace_back('d');

			EXPECT_STREQ("x", deq[0].getString().c_str());
			EXPECT_STREQ("y", deq[1].getString().c_str());
			EXPECT_STREQ("z", deq[2].getString().c_str());

			test_object::clearfunctioncalls();    // clear, b/c the initializer lists have different numbers
			// of calls in different library implementations.

			// insert range from vector
			result = buffer.insert(buffer.end(), vec.begin(), vec.end());

			EXPECT_EQ(4, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(buffer.begin(), result);
			EXPECT_STREQ("a", result->getString().c_str());

			EXPECT_STREQ("a", buffer[0].getString().c_str());
			EXPECT_STREQ("b", buffer[1].getString().c_str());
			EXPECT_STREQ("c", buffer[2].getString().c_str());
			EXPECT_STREQ("d", buffer[3].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(4, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// insert range from const dequeue
			auto cItr = std::find(buffer.begin(), buffer.end(), 'c');

			test_object::clearfunctioncalls();    // I'm not testing the STL algorithm efficiency

			EXPECT_STREQ("c", cItr->getString().c_str());

			result = buffer.insert(cItr, deq.begin(), deq.end());

			EXPECT_EQ(7, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_STREQ("x", result->getString().c_str());
			EXPECT_STREQ("c", (result + deq.size())->getString().c_str());

			EXPECT_STREQ("a", buffer[0].getString().c_str());
			EXPECT_STREQ("b", buffer[1].getString().c_str());
			EXPECT_STREQ("x", buffer[2].getString().c_str());
			EXPECT_STREQ("y", buffer[3].getString().c_str());
			EXPECT_STREQ("z", buffer[4].getString().c_str());
			EXPECT_STREQ("c", buffer[5].getString().c_str());
			EXPECT_STREQ("d", buffer[6].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(3, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(3 * (5 - 1), test_object::getmoveconstructorcalls());
			EXPECT_EQ(3 * (5 - 1) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(3 * (5 - 1), test_object::getdestructorcalls());

			// insert 0 range
			result = buffer.insert(cItr, deq.begin(), deq.begin());

			EXPECT_EQ(7, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_STREQ("x", result->getString().c_str());    // no change
			EXPECT_STREQ("c", (result + deq.size())->getString().c_str());

			EXPECT_STREQ("a", buffer[0].getString().c_str());
			EXPECT_STREQ("b", buffer[1].getString().c_str());
			EXPECT_STREQ("x", buffer[2].getString().c_str());
			EXPECT_STREQ("y", buffer[3].getString().c_str());
			EXPECT_STREQ("z", buffer[4].getString().c_str());
			EXPECT_STREQ("c", buffer[5].getString().c_str());
			EXPECT_STREQ("d", buffer[6].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// insert range > capacity
			result = buffer.insert(cItr, big.begin(), big.end());

			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_STREQ("e", result->getString().c_str());

			EXPECT_STREQ("e", buffer[0].getString().c_str());
			EXPECT_STREQ("f", buffer[1].getString().c_str());
			EXPECT_STREQ("g", buffer[2].getString().c_str());
			EXPECT_STREQ("h", buffer[3].getString().c_str());
			EXPECT_STREQ("i", buffer[4].getString().c_str());
			EXPECT_STREQ("j", buffer[5].getString().c_str());
			EXPECT_STREQ("k", buffer[6].getString().c_str());
			EXPECT_STREQ("l", buffer[7].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(1, test_object::getcopyconstructorcalls());
			EXPECT_EQ(7, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(0, test_object::getdestructorcalls());

			// insert range from self
			result = buffer.insert(buffer.begin() + 6, buffer.begin() + 4, buffer.end());

			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(buffer.begin() + 2, result);
			EXPECT_STREQ("i", result->getString().c_str());

			EXPECT_STREQ("i", buffer[0].getString().c_str());
			EXPECT_STREQ("j", buffer[1].getString().c_str());
			EXPECT_STREQ("i", buffer[2].getString().c_str());
			EXPECT_STREQ("j", buffer[3].getString().c_str());
			EXPECT_STREQ("k", buffer[4].getString().c_str());
			EXPECT_STREQ("l", buffer[5].getString().c_str());
			EXPECT_STREQ("k", buffer[6].getString().c_str());
			EXPECT_STREQ("l", buffer[7].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(4, test_object::getcopyassignmentcalls());
			EXPECT_EQ(4 * (6 - 1), test_object::getmoveconstructorcalls());
			EXPECT_EQ(4 * (6 - 1) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(4 * (6 - 1), test_object::getdestructorcalls());

			// insert into wrapped buffer
			circular_queue_test_wrapper buffer1(8);
			buffer1.emplace_back('a');
			buffer1.emplace_back('b');
			buffer1.emplace_back('c');
			buffer1.emplace_back('d');
			buffer1.emplace_back('e');
			buffer1.emplace_back('f');
			buffer1.emplace_back('g');
			buffer1.emplace_back('h');
			buffer1.emplace_back('i');
			buffer1.emplace_back('j');

			EXPECT_EQ(10, test_object::getconstructorcalls());
			EXPECT_EQ(2, test_object::getdestructorcalls());

			EXPECT_STREQ("c", buffer1[0].getString().c_str());
			EXPECT_STREQ("d", buffer1[1].getString().c_str());
			EXPECT_STREQ("e", buffer1[2].getString().c_str());
			EXPECT_STREQ("f", buffer1[3].getString().c_str());
			EXPECT_STREQ("g", buffer1[4].getString().c_str());
			EXPECT_STREQ("h", buffer1[5].getString().c_str());
			EXPECT_STREQ("i", buffer1[6].getString().c_str());
			EXPECT_STREQ("j", buffer1[7].getString().c_str());

			result = buffer1.insert(buffer1.begin() + 6, deq.begin(), deq.end());

			EXPECT_EQ(8, buffer1.size());
			EXPECT_EQ(8, buffer1.capacity());

			EXPECT_EQ(buffer1.begin() + 3, result);
			EXPECT_STREQ("x", result->getString().c_str());

			EXPECT_STREQ("f", buffer1[0].getString().c_str());
			EXPECT_STREQ("g", buffer1[1].getString().c_str());
			EXPECT_STREQ("h", buffer1[2].getString().c_str());
			EXPECT_STREQ("x", buffer1[3].getString().c_str());
			EXPECT_STREQ("y", buffer1[4].getString().c_str());
			EXPECT_STREQ("z", buffer1[5].getString().c_str());
			EXPECT_STREQ("i", buffer1[6].getString().c_str());
			EXPECT_STREQ("j", buffer1[7].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(3, test_object::getcopyassignmentcalls());
			EXPECT_EQ(3 * (6 - 2), test_object::getmoveconstructorcalls());
			EXPECT_EQ(3 * (6 - 2) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(3 * (6 - 2), test_object::getdestructorcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(35, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(54, test_object::getstringfunctioncalls());
	}

	// This test checks inserting ranges from other containers
	TEST_F(circular_queue_test, InsertInitializerList)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// IL LIST INSERT IS IMPLEMENTED BY FORWARDING TO
			// INSERT (RANGE), SO TESTING CAN BE KEPT SHORT...

			circular_queue_test_wrapper           buffer(8);
			circular_queue_test_wrapper::iterator result;

			// insert into empty buffer
			result = buffer.insert(buffer.end(),
			                       {test_object('a'),
			                        test_object('b'),
			                        test_object('h')});

			EXPECT_EQ(3, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(buffer.begin(), result);
			EXPECT_STREQ("a", result->getString().c_str());

			EXPECT_STREQ("a", buffer[0].getString().c_str());
			EXPECT_STREQ("b", buffer[1].getString().c_str());
			EXPECT_STREQ("h", buffer[2].getString().c_str());

			EXPECT_EQ(3, test_object::getconstructorcalls());        // init list
			EXPECT_EQ(3, test_object::getcopyconstructorcalls());    // init list
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(3, test_object::getdestructorcalls());

			// insert into partially full buffer
			result = buffer.insert(buffer.begin() + 2,
			                       {test_object('c'),
			                        test_object('d'),
			                        test_object('e'),
			                        test_object('f'),
			                        test_object('g')});

			EXPECT_EQ(8, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(buffer.begin() + 2, result);
			EXPECT_STREQ("c", result->getString().c_str());

			EXPECT_STREQ("a", buffer[0].getString().c_str());
			EXPECT_STREQ("b", buffer[1].getString().c_str());
			EXPECT_STREQ("c", buffer[2].getString().c_str());
			EXPECT_STREQ("d", buffer[3].getString().c_str());
			EXPECT_STREQ("e", buffer[4].getString().c_str());
			EXPECT_STREQ("f", buffer[5].getString().c_str());
			EXPECT_STREQ("g", buffer[6].getString().c_str());
			EXPECT_STREQ("h", buffer[7].getString().c_str());

			EXPECT_EQ(5, test_object::getconstructorcalls());        // init list
			EXPECT_EQ(5, test_object::getcopyconstructorcalls());    // init list
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(5 * (6 - 1), test_object::getmoveconstructorcalls());
			EXPECT_EQ(5 * (6 - 1) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(5 * (6 - 1) + 5, test_object::getdestructorcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(8, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(13, test_object::getstringfunctioncalls());
	}

	//
	TEST_F(circular_queue_test, Erase)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(8);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');

			circular_queue_test_wrapper::iterator result;

			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(6, test_object::getconstructorcalls());

			// erasing empty container is undefined == no test

			// erase begin of buffer
			result = buffer.erase(buffer.begin());

			EXPECT_EQ(5, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(buffer.begin(), result);
			EXPECT_STREQ("b", result->getString().c_str());

			EXPECT_STREQ("b", buffer[0].getString().c_str());
			EXPECT_STREQ("c", buffer[1].getString().c_str());
			EXPECT_STREQ("d", buffer[2].getString().c_str());
			EXPECT_STREQ("e", buffer[3].getString().c_str());
			EXPECT_STREQ("f", buffer[4].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(1 * (6 - 1), test_object::getmoveconstructorcalls());
			EXPECT_EQ(1 * (6 - 1) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(1 * (6 - 1) + 1, test_object::getdestructorcalls());    // +1 is the one we asked to erase

			// erasing the end is also undefined ==  no test

			// erase back
			result = buffer.erase(std::prev(buffer.end()));

			EXPECT_EQ(4, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_TRUE(buffer.end() == result);

			EXPECT_STREQ("b", buffer[0].getString().c_str());
			EXPECT_STREQ("c", buffer[1].getString().c_str());
			EXPECT_STREQ("d", buffer[2].getString().c_str());
			EXPECT_STREQ("e", buffer[3].getString().c_str());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // erasing end requires no swaps
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(1, test_object::getdestructorcalls());

			// erase remaining
			for (auto itr = buffer.begin(); itr != buffer.end();)
			{
				itr = buffer.erase(itr);
			}

			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(6, test_object::getmoveconstructorcalls());    // erasing end requires no swaps
			EXPECT_EQ(12, test_object::getmoveassignmentcalls());
			EXPECT_EQ(6 + 4, test_object::getdestructorcalls());

			// Erase wrapped buffer
			buffer.clear();

			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');
			buffer.emplace_back('g');
			buffer.emplace_back('h');
			buffer.emplace_back('i');
			buffer.emplace_back('j');

			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(10, test_object::getconstructorcalls());
			EXPECT_EQ(2, test_object::getdestructorcalls());

			// erasing empty container is undefined == no test

			// erase last 4 of end
			for (auto itr = buffer.end() - 4; itr != buffer.end();)
			{
				itr = buffer.erase(itr);
			}

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(6, test_object::getmoveconstructorcalls());    // erasing end requires no swaps
			EXPECT_EQ(12, test_object::getmoveassignmentcalls());
			EXPECT_EQ(6 + 4, test_object::getdestructorcalls());

			buffer.clear();

			EXPECT_EQ(4, test_object::getdestructorcalls());
		}

		EXPECT_EQ(0, test_object::getconstructorcalls());
		EXPECT_EQ(0, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(10, test_object::getstringfunctioncalls());
	}

	//
	TEST_F(circular_queue_test, EraseRange)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper buffer(8);
			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			buffer.emplace_back('d');
			buffer.emplace_back('e');
			buffer.emplace_back('f');
			buffer.emplace_back('g');
			buffer.emplace_back('h');
			buffer.emplace_back('i');
			buffer.emplace_back('j');

			EXPECT_EQ(2, test_object::getdestructorcalls());

			circular_queue_test_wrapper::iterator result;

			EXPECT_EQ(8, buffer.capacity());
			EXPECT_EQ(10, test_object::getconstructorcalls());

			// erasing empty container is undefined == no test

			// erase last 4 of end
			result = buffer.erase(buffer.end() - 4, buffer.end());

			EXPECT_EQ(4, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // erasing end requires no swaps
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(4, test_object::getdestructorcalls());

			// erase middle two
			result = buffer.erase(buffer.begin() + 1, buffer.end() - 1);

			EXPECT_EQ(2, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(2 * (3 - 1), test_object::getmoveconstructorcalls());
			EXPECT_EQ(2 * (3 - 1) * 2, test_object::getmoveassignmentcalls());
			EXPECT_EQ(2 * (3 - 1) + 2, test_object::getdestructorcalls());

			// erase remaining
			result = buffer.erase(buffer.begin(), buffer.end());

			EXPECT_EQ(0, buffer.size());
			EXPECT_EQ(8, buffer.capacity());

			EXPECT_EQ(0, test_object::getconstructorcalls());
			EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyconstructorcalls());
			EXPECT_EQ(0, test_object::getcopyassignmentcalls());
			EXPECT_EQ(0, test_object::getmoveconstructorcalls());    // erasing end requires no swaps
			EXPECT_EQ(0, test_object::getmoveassignmentcalls());
			EXPECT_EQ(2, test_object::getdestructorcalls());
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

	//
	TEST_F(circular_queue_test, Swap)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue_test_wrapper full(8);
			full.emplace_back('a');
			full.emplace_back('b');
			full.emplace_back('c');
			full.emplace_back('d');
			full.emplace_back('e');
			full.emplace_back('f');
			full.emplace_back('g');
			full.emplace_back('h');
			full.emplace_back('i');
			full.emplace_back('j');

			auto fullData       = full.data();
			auto fullHead       = full.head();
			auto fullTail       = full.tail();
			auto fullHeadParity = full.headParity();
			auto fullTailParity = full.tailParity();

			circular_queue_test_wrapper partial(16);
			partial.emplace_back('a');
			partial.emplace_back('b');
			partial.emplace_back('c');
			partial.emplace_back('d');

			auto partialData       = partial.data();
			auto partialHead       = partial.head();
			auto partialTail       = partial.tail();
			auto partialHeadParity = partial.headParity();
			auto partialTailParity = partial.tailParity();

			full.swap(partial);

			// partial
			EXPECT_EQ(8, partial.capacity());
			EXPECT_EQ(8, partial.size());

			EXPECT_EQ(fullData, partial.data());
			EXPECT_EQ(fullHead, partial.head());
			EXPECT_EQ(fullTail, partial.tail());
			EXPECT_EQ(fullHeadParity, partial.headParity());
			EXPECT_EQ(fullTailParity, partial.tailParity());

			EXPECT_STREQ("c", partial[0].getString().c_str());
			EXPECT_STREQ("d", partial[1].getString().c_str());
			EXPECT_STREQ("e", partial[2].getString().c_str());
			EXPECT_STREQ("f", partial[3].getString().c_str());
			EXPECT_STREQ("g", partial[4].getString().c_str());
			EXPECT_STREQ("h", partial[5].getString().c_str());
			EXPECT_STREQ("i", partial[6].getString().c_str());
			EXPECT_STREQ("j", partial[7].getString().c_str());

			// full
			EXPECT_EQ(16, full.capacity());
			EXPECT_EQ(4, full.size());

			EXPECT_EQ(partialData, full.data());
			EXPECT_EQ(partialHead, full.head());
			EXPECT_EQ(partialTail, full.tail());
			EXPECT_EQ(partialHeadParity, full.headParity());
			EXPECT_EQ(partialTailParity, full.tailParity());

			EXPECT_STREQ("a", full[0].getString().c_str());
			EXPECT_STREQ("b", full[1].getString().c_str());
			EXPECT_STREQ("c", full[2].getString().c_str());
			EXPECT_STREQ("d", full[3].getString().c_str());
		}
	}

	TEST_F(circular_queue_test, getAllocator)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			// test that this compiles basically. It's just an accessor.
			std::allocator<test_object> myAllocator;

			circular_queue<test_object> buffer(8, myAllocator);

			auto alloc = buffer.get_allocator();
			EXPECT_EQ(alloc, myAllocator);
		}
	}

	TEST_F(circular_queue_test, copy)
	{
		// test that data can be memcopied. Do it for a basic type
		circular_queue<int> buffer(100);

		// wrap the buffer, then pop some elements from both sides
		for (int i = 0; i < 150; ++i)
		{
			buffer.push_back(i);
		}

		EXPECT_EQ(100, buffer.size());

		for (int i = 0; i < 5; ++i)
		{
			buffer.pop_front();
			buffer.pop_back();
		}

		EXPECT_EQ(90, buffer.size());

		int val = 55;
		for (int i = 0; i < buffer.size(); ++i)
		{
			EXPECT_EQ(val++, buffer[i]);
		}

		int output[90];
		buffer.copy(buffer.begin(), buffer.end(), &output[0], &memcpy);

		val = 55;
		for (int i = 0; i < 90; ++i)
		{
			EXPECT_EQ(val++, output[i]);
		}

		// try with full but unwrapped buffer
		circular_queue<int> buffer2(75, 5);
		EXPECT_EQ(75, buffer2.size());

		buffer2.copy(buffer2.begin(), buffer2.end(), &output[0], &memcpy);
		for (int i = 0; i < 75; ++i)
		{
			EXPECT_EQ(5, output[i]);
		}

		// try with not-full buffer
		circular_queue<int> buffer3(50, 2);
		EXPECT_EQ(50, buffer3.size());

		buffer3.copy(buffer3.begin() + 10, buffer3.end() - 10, &output[0], &memcpy);
		for (int i = 0; i < 30; ++i)
		{
			EXPECT_EQ(2, output[i]);
		}
		for (int i = 30; i < 75; ++i)
		{
			EXPECT_EQ(5, output[i]);
		}
	}

	// This test ensures iterators can be created without blowing things up. The actual iterator
	// functionality is tested separately.
	TEST_F(circular_queue_test, Iterators)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			circular_queue<test_object> buffer(16);
			EXPECT_EQ(16, buffer.capacity());

			buffer.emplace_back('a');
			buffer.emplace_back('b');
			buffer.emplace_back('c');
			EXPECT_EQ(3, buffer.size());

			circular_queue<test_object>::iterator               itrBegin             = buffer.begin();
			circular_queue<test_object>::iterator               itrEnd               = buffer.end();
			circular_queue<test_object>::reverse_iterator       reverseItrBegin      = buffer.rbegin();
			circular_queue<test_object>::reverse_iterator       reverseItrEnd        = buffer.rend();
			circular_queue<test_object>::const_iterator         constItrBegin        = buffer.cbegin();
			circular_queue<test_object>::const_iterator         constItrEnd          = buffer.cend();
			circular_queue<test_object>::const_reverse_iterator constReverseItrBegin = buffer.crbegin();
			circular_queue<test_object>::const_reverse_iterator constReverseItrEnd   = buffer.crend();

			// Check that the objects pointed to by our const pointers are in fact const
			EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*constItrBegin)>::type>::value);
			EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*constItrEnd)>::type>::value);
			EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*constReverseItrBegin)>::type>::value);
			EXPECT_TRUE(std::is_const<typename std::remove_reference<decltype(*constReverseItrEnd)>::type>::value);

			// Check that the objects pointed to by the mutable iterators are not const
			EXPECT_FALSE(std::is_const<typename std::remove_reference<decltype(*itrBegin)>::type>::value);
			EXPECT_FALSE(std::is_const<typename std::remove_reference<decltype(*itrEnd)>::type>::value);
			EXPECT_FALSE(std::is_const<typename std::remove_reference<decltype(*reverseItrBegin)>::type>::value);
			EXPECT_FALSE(std::is_const<typename std::remove_reference<decltype(*reverseItrEnd)>::type>::value);

			// check that the iterators are dereferenceable they point to the right stuff
			// This causes a construct/destruct and 8 copy assigns.
			--itrEnd;
			--reverseItrEnd;
			--constItrEnd;
			--constReverseItrEnd;

			EXPECT_STREQ((*itrBegin).getString().c_str(), "a");
			EXPECT_STREQ((*itrEnd).getString().c_str(), "c");
			EXPECT_STREQ((*reverseItrBegin).getString().c_str(), "c");
			EXPECT_STREQ((*reverseItrEnd).getString().c_str(), "a");
			EXPECT_STREQ((*constItrBegin).getString().c_str(), "a");
			EXPECT_STREQ((*constItrEnd).getString().c_str(), "c");
			EXPECT_STREQ((*constReverseItrBegin).getString().c_str(), "c");
			EXPECT_STREQ((*constReverseItrEnd).getString().c_str(), "a");

			EXPECT_STREQ((&itrBegin)->getString().c_str(), "a");
			EXPECT_STREQ((&itrEnd)->getString().c_str(), "c");
			EXPECT_STREQ((&constItrBegin)->getString().c_str(), "a");
			EXPECT_STREQ((&constItrEnd)->getString().c_str(), "c");

			EXPECT_STREQ(itrBegin->getString().c_str(), "a");
			EXPECT_STREQ((itrEnd)->getString().c_str(), "c");
			EXPECT_STREQ((reverseItrBegin)->getString().c_str(), "c");
			EXPECT_STREQ((reverseItrEnd)->getString().c_str(), "a");
			EXPECT_STREQ((constItrBegin)->getString().c_str(), "a");
			EXPECT_STREQ((constItrEnd)->getString().c_str(), "c");
			EXPECT_STREQ((constReverseItrBegin)->getString().c_str(), "c");
			EXPECT_STREQ((constReverseItrEnd)->getString().c_str(), "a");
		}

		// check for leaks or extraneous object creation
		EXPECT_EQ(3, test_object::getconstructorcalls());
		EXPECT_EQ(3, test_object::getdestructorcalls());
		EXPECT_EQ(0, test_object::getdefaultconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveconstructorcalls());
		EXPECT_EQ(0, test_object::getcopyconstructorcalls());
		EXPECT_EQ(0, test_object::getmoveassignmentcalls());
		EXPECT_EQ(0, test_object::getcopyassignmentcalls());
		EXPECT_EQ(0, test_object::gettestfunctioncalls());
		EXPECT_EQ(20, test_object::getstringfunctioncalls());
	}

	TEST_F(circular_queue_iterator_test, IteratorDefaultConstructor)
	{
		circular_queue<test_object>::iterator               itr;
		circular_queue<test_object>::reverse_iterator       ritr;
		circular_queue<test_object>::const_iterator         citr;
		circular_queue<test_object>::const_reverse_iterator critr;
	}

	TEST_F(circular_queue_iterator_test, IteratorCopyConstructor)
	{
		circular_queue<test_object>::iterator               itr   = fullBuffer->begin();
		circular_queue<test_object>::reverse_iterator       ritr  = fullBuffer->rbegin();
		circular_queue<test_object>::const_iterator         citr  = fullBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator critr = fullBuffer->crbegin();

		// copy construction
		auto itr2(itr);
		auto citr2(citr);
		auto ritr2(ritr);
		auto critr2(critr);

		EXPECT_EQ(itr2, itr);
		EXPECT_EQ(citr2, citr);
		EXPECT_EQ(ritr2, ritr);
		EXPECT_EQ(critr2, critr);

		// mutable to const conversion
		auto citr3(itr);
		auto critr3(ritr);

		EXPECT_EQ(citr3, itr);
		EXPECT_EQ(critr3, ritr);
	}

	TEST_F(circular_queue_iterator_test, IteratorDereference)
	{
		circular_queue_test_wrapper::iterator               itr   = fullBuffer->begin();
		circular_queue_test_wrapper::reverse_iterator       ritr  = fullBuffer->rbegin();
		circular_queue_test_wrapper::const_iterator         citr  = fullBuffer->cbegin();
		circular_queue_test_wrapper::const_reverse_iterator critr = fullBuffer->crbegin();

		EXPECT_EQ(test_object('a'), *itr);
		EXPECT_EQ(test_object('a'), *citr);
		EXPECT_EQ(test_object('p'), *ritr);
		EXPECT_EQ(test_object('p'), *critr);

		EXPECT_EQ("a", itr->getString());
		EXPECT_EQ("a", citr->getString());
		EXPECT_EQ("p", ritr->getString());
		EXPECT_EQ("p", critr->getString());

		circular_queue<char>                   test{'a', 'b', 'c', 'd'};
		circular_queue<char>::reverse_iterator rvitr = test.rbegin();
		EXPECT_EQ('b', rvitr[2]);

		EXPECT_EQ("c", itr[2].getString());
		EXPECT_EQ("c", citr[2].getString());
		EXPECT_EQ("n", ritr[2].getString());
		EXPECT_EQ("n", critr[2].getString());

		EXPECT_EQ(fullBuffer->data(), &itr);
		EXPECT_EQ(fullBuffer->data(), &citr);
		// reverse iterators don't have operator&
	}

	TEST_F(circular_queue_iterator_test, IteratorOperatorEqual)
	{
		circular_queue<test_object>::iterator               fullitr   = fullBuffer->begin();
		circular_queue<test_object>::reverse_iterator       fullritr  = fullBuffer->rbegin();
		circular_queue<test_object>::const_iterator         fullcitr  = fullBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator fullrcitr = fullBuffer->crbegin();

		for (auto itr = ++fullBuffer->begin(); itr != fullBuffer->end(); ++itr)
		{
			EXPECT_FALSE(fullitr == itr);
			EXPECT_FALSE(fullcitr == itr);
		}

		for (auto itr = ++fullBuffer->rbegin(); itr != fullBuffer->rend(); ++itr)
		{
			EXPECT_FALSE(fullritr == itr);
			EXPECT_FALSE(fullrcitr == itr);
		}

		circular_queue<test_object>::iterator               emptyitr   = emptyBuffer->begin();
		circular_queue<test_object>::reverse_iterator       emptyritr  = emptyBuffer->rbegin();
		circular_queue<test_object>::const_iterator         emptycitr  = emptyBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator emptyrcitr = emptyBuffer->crbegin();

		for (auto itr = ++emptyBuffer->begin(); itr != emptyBuffer->end(); ++itr)
		{
			EXPECT_FALSE(emptyitr == itr);
			EXPECT_FALSE(emptycitr == itr);
		}

		for (auto itr = ++emptyBuffer->rbegin(); itr != emptyBuffer->rend(); ++itr)
		{
			EXPECT_FALSE(emptyritr == itr);
			EXPECT_FALSE(emptyrcitr == itr);
		}

		circular_queue<test_object>::iterator               partialitr   = partialBuffer->begin();
		circular_queue<test_object>::reverse_iterator       partialritr  = partialBuffer->rbegin();
		circular_queue<test_object>::const_iterator         partialcitr  = partialBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator partialrcitr = partialBuffer->crbegin();

		for (auto itr = ++partialBuffer->begin(); itr != partialBuffer->end(); ++itr)
		{
			EXPECT_FALSE(partialitr == itr);
			EXPECT_FALSE(partialcitr == itr);
		}

		for (auto itr = ++partialBuffer->rbegin(); itr != partialBuffer->rend(); ++itr)
		{
			EXPECT_FALSE(partialritr == itr);
			EXPECT_FALSE(partialrcitr == itr);
		}

		circular_queue<test_object>::iterator               fullOverflowitr   = fullOverflowBuffer->begin();
		circular_queue<test_object>::reverse_iterator       fullOverflowritr  = fullOverflowBuffer->rbegin();
		circular_queue<test_object>::const_iterator         fullOverflowcitr  = fullOverflowBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator fullOverflowrcitr = fullOverflowBuffer->crbegin();

		for (auto itr = ++fullOverflowBuffer->begin(); itr != fullOverflowBuffer->end(); ++itr)
		{
			EXPECT_FALSE(fullOverflowitr == itr);
			EXPECT_FALSE(fullOverflowcitr == itr);
		}

		for (auto itr = ++fullOverflowBuffer->rbegin(); itr != fullOverflowBuffer->rend(); ++itr)
		{
			EXPECT_FALSE(fullOverflowritr == itr);
			EXPECT_FALSE(fullOverflowrcitr == itr);
		}

		circular_queue<test_object>::iterator               partialOverflowitr   = partialOverflowBuffer->begin();
		circular_queue<test_object>::reverse_iterator       partialOverflowritr  = partialOverflowBuffer->rbegin();
		circular_queue<test_object>::const_iterator         partialOverflowcitr  = partialOverflowBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator partialOverflowrcitr = partialOverflowBuffer->crbegin();

		for (auto itr = ++partialOverflowBuffer->begin(); itr != partialOverflowBuffer->end(); ++itr)
		{
			EXPECT_FALSE(partialOverflowitr == itr);
			EXPECT_FALSE(partialOverflowcitr == itr);
		}

		for (auto itr = ++partialOverflowBuffer->rbegin(); itr != partialOverflowBuffer->rend(); ++itr)
		{
			EXPECT_FALSE(partialOverflowritr == itr);
			EXPECT_FALSE(partialOverflowrcitr == itr);
		}
	}

	TEST_F(circular_queue_iterator_test, IteratorOperatorNotEqual)
	{
		circular_queue<test_object>::iterator               fullitr   = fullBuffer->begin();
		circular_queue<test_object>::reverse_iterator       fullritr  = fullBuffer->rbegin();
		circular_queue<test_object>::const_iterator         fullcitr  = fullBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator fullrcitr = fullBuffer->crbegin();

		for (auto itr = ++fullBuffer->begin(); itr != fullBuffer->end(); ++itr)
		{
			EXPECT_TRUE(fullitr != itr);
			EXPECT_TRUE(fullcitr != itr);
		}

		for (auto itr = ++fullBuffer->rbegin(); itr != fullBuffer->rend(); ++itr)
		{
			EXPECT_TRUE(fullritr != itr);
			EXPECT_TRUE(fullrcitr != itr);
		}

		circular_queue<test_object>::iterator               emptyitr   = emptyBuffer->begin();
		circular_queue<test_object>::reverse_iterator       emptyritr  = emptyBuffer->rbegin();
		circular_queue<test_object>::const_iterator         emptycitr  = emptyBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator emptyrcitr = emptyBuffer->crbegin();

		for (auto itr = ++emptyBuffer->begin(); itr != emptyBuffer->end(); ++itr)
		{
			EXPECT_TRUE(emptyitr != itr);
			EXPECT_TRUE(emptycitr != itr);
		}

		for (auto itr = ++emptyBuffer->rbegin(); itr != emptyBuffer->rend(); ++itr)
		{
			EXPECT_TRUE(emptyritr != itr);
			EXPECT_TRUE(emptyrcitr != itr);
		}

		circular_queue<test_object>::iterator               partialitr   = partialBuffer->begin();
		circular_queue<test_object>::reverse_iterator       partialritr  = partialBuffer->rbegin();
		circular_queue<test_object>::const_iterator         partialcitr  = partialBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator partialrcitr = partialBuffer->crbegin();

		for (auto itr = ++partialBuffer->begin(); itr != partialBuffer->end(); ++itr)
		{
			EXPECT_TRUE(partialitr != itr);
			EXPECT_TRUE(partialcitr != itr);
		}

		for (auto itr = ++partialBuffer->rbegin(); itr != partialBuffer->rend(); ++itr)
		{
			EXPECT_TRUE(partialritr != itr);
			EXPECT_TRUE(partialrcitr != itr);
		}

		circular_queue<test_object>::iterator               fullOverflowitr   = fullOverflowBuffer->begin();
		circular_queue<test_object>::reverse_iterator       fullOverflowritr  = fullOverflowBuffer->rbegin();
		circular_queue<test_object>::const_iterator         fullOverflowcitr  = fullOverflowBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator fullOverflowrcitr = fullOverflowBuffer->crbegin();

		for (auto itr = ++fullOverflowBuffer->begin(); itr != fullOverflowBuffer->end(); ++itr)
		{
			EXPECT_TRUE(fullOverflowitr != itr);
			EXPECT_TRUE(fullOverflowcitr != itr);
		}

		for (auto itr = ++fullOverflowBuffer->rbegin(); itr != fullOverflowBuffer->rend(); ++itr)
		{
			EXPECT_TRUE(fullOverflowritr != itr);
			EXPECT_TRUE(fullOverflowrcitr != itr);
		}

		circular_queue<test_object>::iterator               partialOverflowitr   = partialOverflowBuffer->begin();
		circular_queue<test_object>::reverse_iterator       partialOverflowritr  = partialOverflowBuffer->rbegin();
		circular_queue<test_object>::const_iterator         partialOverflowcitr  = partialOverflowBuffer->cbegin();
		circular_queue<test_object>::const_reverse_iterator partialOverflowrcitr = partialOverflowBuffer->crbegin();

		for (auto itr = ++partialOverflowBuffer->begin(); itr != partialOverflowBuffer->end(); ++itr)
		{
			EXPECT_TRUE(partialOverflowitr != itr);
			EXPECT_TRUE(partialOverflowcitr != itr);
		}

		for (auto itr = ++partialOverflowBuffer->rbegin(); itr != partialOverflowBuffer->rend(); ++itr)
		{
			EXPECT_TRUE(partialOverflowritr != itr);
			EXPECT_TRUE(partialOverflowrcitr != itr);
		}
	}

	TEST_F(circular_queue_iterator_test, operatorPlusPlus)
	{
		size_t loopCounter;
		char   c;

		// prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = fullBuffer->begin(); itr != fullBuffer->end(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->begin(); itr != emptyBuffer->end(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = partialBuffer->begin(); itr != partialBuffer->end(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'n';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->begin(); itr != partialOverflowBuffer->end(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'k';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->begin(); itr != fullOverflowBuffer->end(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = fullBuffer->begin(); itr != fullBuffer->end(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->begin(); itr != emptyBuffer->end(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = partialBuffer->begin(); itr != partialBuffer->end(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'n';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->begin(); itr != partialOverflowBuffer->end(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'k';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->begin(); itr != fullOverflowBuffer->end(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = fullBuffer->cbegin(); itr != fullBuffer->cend(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->cbegin(); itr != emptyBuffer->cend(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = partialBuffer->cbegin(); itr != partialBuffer->cend(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'n';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->cbegin(); itr != partialOverflowBuffer->cend(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'k';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->cbegin(); itr != fullOverflowBuffer->cend(); ++itr, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = fullBuffer->cbegin(); itr != fullBuffer->cend(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->cbegin(); itr != emptyBuffer->cend(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = partialBuffer->cbegin(); itr != partialBuffer->cend(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'n';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->cbegin(); itr != partialOverflowBuffer->cend(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'k';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->cbegin(); itr != fullOverflowBuffer->cend(); itr++, loopCounter++, c++)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// reverse
		// prefix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = fullBuffer->rbegin(); itr != fullBuffer->rend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->rbegin(); itr != emptyBuffer->rend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'd';
		loopCounter = 0;
		for (auto itr = partialBuffer->rbegin(); itr != partialBuffer->rend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'w';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->rbegin(); itr != partialOverflowBuffer->rend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'z';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->rbegin(); itr != fullOverflowBuffer->rend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// postfix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = fullBuffer->rbegin(); itr != fullBuffer->rend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->rbegin(); itr != emptyBuffer->rend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'd';
		loopCounter = 0;
		for (auto itr = partialBuffer->rbegin(); itr != partialBuffer->rend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'w';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->rbegin(); itr != partialOverflowBuffer->rend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'z';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->rbegin(); itr != fullOverflowBuffer->rend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// prefix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = fullBuffer->crbegin(); itr != fullBuffer->crend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->crbegin(); itr != emptyBuffer->crend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'd';
		loopCounter = 0;
		for (auto itr = partialBuffer->crbegin(); itr != partialBuffer->crend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'w';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->crbegin(); itr != partialOverflowBuffer->crend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'z';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->crbegin(); itr != fullOverflowBuffer->crend(); ++itr, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// postfix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = fullBuffer->crbegin(); itr != fullBuffer->crend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		c           = 'a';
		loopCounter = 0;
		for (auto itr = emptyBuffer->crbegin(); itr != emptyBuffer->crend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		c           = 'd';
		loopCounter = 0;
		for (auto itr = partialBuffer->crbegin(); itr != partialBuffer->crend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		c           = 'w';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->crbegin(); itr != partialOverflowBuffer->crend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'z';
		loopCounter = 0;
		for (auto itr = fullOverflowBuffer->crbegin(); itr != fullOverflowBuffer->crend(); itr++, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());
	}

	TEST_F(circular_queue_iterator_test, operatorMinusMinus)
	{
		size_t loopCounter;
		char   c;

		// full prefix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = --fullBuffer->end(); itr != --fullBuffer->begin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// full postfix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = --fullBuffer->end(); itr != --fullBuffer->begin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// empty prefix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->end(); itr != --emptyBuffer->begin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// empty postfix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->end(); itr != --emptyBuffer->begin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// partial prefix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->end(); itr != --partialBuffer->begin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->end(); itr != --partialBuffer->begin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// fullOverflow prefix
		c           = 'z';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->end(); itr != --fullOverflowBuffer->begin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// fullOverflow postfix
		c           = 'z';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->end(); itr != --fullOverflowBuffer->begin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// partial prefix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->cend(); itr != --partialBuffer->cbegin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->cend(); itr != --partialBuffer->cbegin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// full prefix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = --fullBuffer->cend(); itr != --fullBuffer->cbegin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// full postfix
		c           = 'p';
		loopCounter = 0;
		for (auto itr = --fullBuffer->cend(); itr != --fullBuffer->cbegin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// empty prefix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->cend(); itr != --emptyBuffer->cbegin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// empty postfix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->cend(); itr != --emptyBuffer->cbegin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// partial prefix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->cend(); itr != --partialBuffer->cbegin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->cend(); itr != --partialBuffer->cbegin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// fullOverflow prefix
		c           = 'z';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->cend(); itr != --fullOverflowBuffer->cbegin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// fullOverflow postfix
		c           = 'z';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->cend(); itr != --fullOverflowBuffer->cbegin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// partial prefix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->cend(); itr != --partialBuffer->cbegin(); --itr, loopCounter++, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'd';
		loopCounter = 0;
		for (auto itr = --partialBuffer->cend(); itr != --partialBuffer->cbegin(); itr--, loopCounter++, c--)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// reverse
		// full prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --fullBuffer->rend(); itr != --fullBuffer->rbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// full postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --fullBuffer->rend(); itr != --fullBuffer->rbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// empty prefix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->rend(); itr != --emptyBuffer->rbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// empty postfix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->rend(); itr != --emptyBuffer->rbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// partial prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->rend(); itr != --partialBuffer->rbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->rend(); itr != --partialBuffer->rbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// fullOverflow prefix
		c           = 'k';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->rend(); itr != --fullOverflowBuffer->rbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// fullOverflow postfix
		c           = 'k';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->rend(); itr != --fullOverflowBuffer->rbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// partial prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->crend(); itr != --partialBuffer->crbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->crend(); itr != --partialBuffer->crbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// full prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --fullBuffer->crend(); itr != --fullBuffer->crbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// full postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --fullBuffer->crend(); itr != --fullBuffer->crbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullBuffer->size());

		// empty prefix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->crend(); itr != --emptyBuffer->crbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// empty postfix
		c           = ' ';
		loopCounter = 0;
		for (auto itr = --emptyBuffer->crend(); itr != --emptyBuffer->crbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, emptyBuffer->size());

		// partial prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->crend(); itr != --partialBuffer->crbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->crend(); itr != --partialBuffer->crbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// fullOverflow prefix
		c           = 'k';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->crend(); itr != --fullOverflowBuffer->crbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// fullOverflow postfix
		c           = 'k';
		loopCounter = 0;
		for (auto itr = --fullOverflowBuffer->crend(); itr != --fullOverflowBuffer->crbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, fullOverflowBuffer->size());

		// partial prefix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->crend(); itr != --partialBuffer->crbegin(); --itr, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());

		// partial postfix
		c           = 'a';
		loopCounter = 0;
		for (auto itr = --partialBuffer->crend(); itr != --partialBuffer->crbegin(); itr--, loopCounter++, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialBuffer->size());
	}

	TEST_F(circular_queue_iterator_test, operatorMinusIterator)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());

		// extra scope to support memory leak detection
		{
			auto emptyFirst = emptyBuffer->begin();
			auto emptyLast  = emptyBuffer->end();

			EXPECT_EQ(emptyBuffer->size(), emptyLast - emptyFirst);
			EXPECT_EQ(std::distance(emptyFirst, emptyLast), emptyLast - emptyFirst);

			auto fullFirst = fullBuffer->begin();
			auto fullLast  = fullBuffer->end();

			EXPECT_EQ(fullBuffer->size(), fullLast - fullFirst);
			EXPECT_EQ(std::distance(fullFirst, fullLast), fullLast - fullFirst);

			auto partialFirst = partialBuffer->begin();
			auto partialLast  = partialBuffer->end();

			EXPECT_EQ(partialBuffer->size(), partialLast - partialFirst);
			EXPECT_EQ(std::distance(partialFirst, partialLast), partialLast - partialFirst);

			auto partialOverflowFirst = partialOverflowBuffer->begin();
			auto partialOverflowLast  = partialOverflowBuffer->end();

			EXPECT_EQ(partialOverflowBuffer->size(), partialOverflowLast - partialOverflowFirst);
			EXPECT_EQ(std::distance(partialOverflowFirst, partialOverflowLast), partialOverflowLast - partialOverflowFirst);

			auto fullOverflowFirst = fullOverflowBuffer->begin();
			auto fullOverflowLast  = fullOverflowBuffer->end();

			EXPECT_EQ(fullOverflowBuffer->size(), fullOverflowLast - fullOverflowFirst);
			EXPECT_EQ(std::distance(fullOverflowFirst, fullOverflowLast), fullOverflowLast - fullOverflowFirst);

			auto emptycFirst = emptyBuffer->cbegin();
			auto emptycLast  = emptyBuffer->cend();

			EXPECT_EQ(emptyBuffer->size(), emptycLast - emptycFirst);
			EXPECT_EQ(std::distance(emptycFirst, emptycLast), emptycLast - emptycFirst);

			auto fullcFirst = fullBuffer->cbegin();
			auto fullcLast  = fullBuffer->cend();

			EXPECT_EQ(fullBuffer->size(), fullcLast - fullcFirst);
			EXPECT_EQ(std::distance(fullcFirst, fullcLast), fullcLast - fullcFirst);

			auto partialcFirst = partialBuffer->cbegin();
			auto partialcLast  = partialBuffer->cend();

			EXPECT_EQ(partialBuffer->size(), partialcLast - partialcFirst);
			EXPECT_EQ(std::distance(partialcFirst, partialcLast), partialcLast - partialcFirst);

			auto partialOverflowcFirst = partialOverflowBuffer->cbegin();
			auto partialOverflowcLast  = partialOverflowBuffer->cend();

			EXPECT_EQ(partialOverflowBuffer->size(), partialOverflowcLast - partialOverflowcFirst);
			EXPECT_EQ(std::distance(partialOverflowcFirst, partialOverflowcLast), partialOverflowcLast - partialOverflowcFirst);

			auto fullOverflowcFirst = fullOverflowBuffer->cbegin();
			auto fullOverflowcLast  = fullOverflowBuffer->cend();

			EXPECT_EQ(fullOverflowBuffer->size(), fullOverflowcLast - fullOverflowcFirst);
			EXPECT_EQ(std::distance(fullOverflowcFirst, fullOverflowcLast), fullOverflowcLast - fullOverflowcFirst);

			auto emptyrFirst = emptyBuffer->rbegin();
			auto emptyrLast  = emptyBuffer->rend();

			EXPECT_EQ(emptyBuffer->size(), emptyrLast - emptyrFirst);
			EXPECT_EQ(std::distance(emptyrFirst, emptyrLast), emptyrLast - emptyrFirst);

			auto fullrFirst = fullBuffer->rbegin();
			auto fullrLast  = fullBuffer->rend();

			EXPECT_EQ(fullBuffer->size(), fullrLast - fullrFirst);
			EXPECT_EQ(std::distance(fullrFirst, fullrLast), fullrLast - fullrFirst);

			auto partialrFirst = partialBuffer->rbegin();
			auto partialrLast  = partialBuffer->rend();

			EXPECT_EQ(partialBuffer->size(), partialrLast - partialrFirst);
			EXPECT_EQ(std::distance(partialrFirst, partialrLast), partialrLast - partialrFirst);

			auto partialOverflowrFirst = partialOverflowBuffer->rbegin();
			auto partialOverflowrLast  = partialOverflowBuffer->rend();

			EXPECT_EQ(partialOverflowBuffer->size(), partialOverflowrLast - partialOverflowrFirst);
			EXPECT_EQ(std::distance(partialOverflowrFirst, partialOverflowrLast), partialOverflowrLast - partialOverflowrFirst);

			auto fullOverflowrFirst = fullOverflowBuffer->rbegin();
			auto fullOverflowrLast  = fullOverflowBuffer->rend();

			EXPECT_EQ(fullOverflowBuffer->size(), fullOverflowrLast - fullOverflowrFirst);
			EXPECT_EQ(std::distance(fullOverflowrFirst, fullOverflowrLast), fullOverflowrLast - fullOverflowrFirst);

			auto emptycrFirst = emptyBuffer->crbegin();
			auto emptycrLast  = emptyBuffer->crend();

			EXPECT_EQ(emptyBuffer->size(), emptycrLast - emptycrFirst);
			EXPECT_EQ(std::distance(emptycrFirst, emptycrLast), emptycrLast - emptycrFirst);

			auto fullcrFirst = fullBuffer->crbegin();
			auto fullcrLast  = fullBuffer->crend();

			EXPECT_EQ(fullBuffer->size(), fullcrLast - fullcrFirst);
			EXPECT_EQ(std::distance(fullcrFirst, fullcrLast), fullcrLast - fullcrFirst);

			auto partialcrFirst = partialBuffer->crbegin();
			auto partialcrLast  = partialBuffer->crend();

			EXPECT_EQ(partialBuffer->size(), partialcrLast - partialcrFirst);
			EXPECT_EQ(std::distance(partialcrFirst, partialcrLast), partialcrLast - partialcrFirst);

			auto partialOverflowcrFirst = partialOverflowBuffer->crbegin();
			auto partialOverflowcrLast  = partialOverflowBuffer->crend();

			EXPECT_EQ(partialOverflowBuffer->size(), partialOverflowcrLast - partialOverflowcrFirst);
			EXPECT_EQ(std::distance(partialOverflowcrFirst, partialOverflowcrLast), partialOverflowcrLast - partialOverflowcrFirst);

			auto fullOverflowcrFirst = fullOverflowBuffer->crbegin();
			auto fullOverflowcrLast  = fullOverflowBuffer->crend();

			EXPECT_EQ(fullOverflowBuffer->size(), fullOverflowcrLast - fullOverflowcrFirst);
			EXPECT_EQ(std::distance(fullOverflowcrFirst, fullOverflowcrLast), fullOverflowcrLast - fullOverflowcrFirst);
		}
	}

	TEST_F(circular_queue_iterator_test, negativeWraparound)
	{
		size_t loopCounter;
		char   c;

		c           = 'w';
		loopCounter = 0;
		for (auto itr = --partialOverflowBuffer->end(); itr != --partialOverflowBuffer->begin(); itr += -1, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'w';
		loopCounter = 0;
		for (auto itr = --partialOverflowBuffer->cend(); itr != --partialOverflowBuffer->cbegin(); itr += -1, ++loopCounter, --c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'n';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->begin(); itr != partialOverflowBuffer->end(); itr -= -1, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());

		c           = 'n';
		loopCounter = 0;
		for (auto itr = partialOverflowBuffer->cbegin(); itr != partialOverflowBuffer->cend(); itr -= -1, ++loopCounter, ++c)
		{
			EXPECT_EQ(*itr, test_object(c));
		}
		EXPECT_EQ(loopCounter, partialOverflowBuffer->size());
	}
	TEST_F(circular_queue_iterator_test, operatorPlusN)
	{
		auto itr  = partialOverflowBuffer->begin();
		auto citr = partialOverflowBuffer->cbegin();

		auto itr2  = itr + 5;
		auto citr2 = citr + 5;

		EXPECT_STREQ("s", itr2->getString().c_str());
		EXPECT_STREQ("s", citr2->getString().c_str());
	}

	TEST_F(circular_queue_iterator_test, relationalOperators)
	{
		auto itr  = partialOverflowBuffer->begin();
		auto citr = partialOverflowBuffer->cbegin();

		EXPECT_FALSE(itr < partialOverflowBuffer->begin());
		EXPECT_TRUE(itr <= partialOverflowBuffer->begin());
		EXPECT_FALSE(itr > partialOverflowBuffer->begin());
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin());
		EXPECT_TRUE(itr == partialOverflowBuffer->begin());
		EXPECT_FALSE(itr != partialOverflowBuffer->begin());

		EXPECT_TRUE(itr < partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr <= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr > partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(itr < partialOverflowBuffer->end());
		EXPECT_TRUE(itr <= partialOverflowBuffer->end());
		EXPECT_FALSE(itr > partialOverflowBuffer->end());
		EXPECT_FALSE(itr >= partialOverflowBuffer->end());
		EXPECT_FALSE(itr == partialOverflowBuffer->end());
		EXPECT_TRUE(itr != partialOverflowBuffer->end());

		itr += 4;

		EXPECT_FALSE(itr < partialOverflowBuffer->begin());
		EXPECT_FALSE(itr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(itr > partialOverflowBuffer->begin());
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(itr == partialOverflowBuffer->begin());
		EXPECT_TRUE(itr != partialOverflowBuffer->begin());

		EXPECT_TRUE(itr < partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr <= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr > partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(itr < partialOverflowBuffer->end());
		EXPECT_TRUE(itr <= partialOverflowBuffer->end());
		EXPECT_FALSE(itr > partialOverflowBuffer->end());
		EXPECT_FALSE(itr >= partialOverflowBuffer->end());
		EXPECT_FALSE(itr == partialOverflowBuffer->end());
		EXPECT_TRUE(itr != partialOverflowBuffer->end());

		++itr;

		EXPECT_FALSE(itr < partialOverflowBuffer->begin());
		EXPECT_FALSE(itr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(itr > partialOverflowBuffer->begin());
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(itr == partialOverflowBuffer->begin());
		EXPECT_TRUE(itr != partialOverflowBuffer->begin());

		EXPECT_FALSE(itr < partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr <= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr > partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr == partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(itr < partialOverflowBuffer->end());
		EXPECT_TRUE(itr <= partialOverflowBuffer->end());
		EXPECT_FALSE(itr > partialOverflowBuffer->end());
		EXPECT_FALSE(itr >= partialOverflowBuffer->end());
		EXPECT_FALSE(itr == partialOverflowBuffer->end());
		EXPECT_TRUE(itr != partialOverflowBuffer->end());

		++itr;

		EXPECT_FALSE(itr < partialOverflowBuffer->begin());
		EXPECT_FALSE(itr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(itr > partialOverflowBuffer->begin());
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(itr == partialOverflowBuffer->begin());
		EXPECT_TRUE(itr != partialOverflowBuffer->begin());

		EXPECT_FALSE(itr < partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr <= partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr > partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(itr < partialOverflowBuffer->end());
		EXPECT_TRUE(itr <= partialOverflowBuffer->end());
		EXPECT_FALSE(itr > partialOverflowBuffer->end());
		EXPECT_FALSE(itr >= partialOverflowBuffer->end());
		EXPECT_FALSE(itr == partialOverflowBuffer->end());
		EXPECT_TRUE(itr != partialOverflowBuffer->end());

		itr = partialOverflowBuffer->end();

		EXPECT_FALSE(itr < partialOverflowBuffer->begin());
		EXPECT_FALSE(itr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(itr > partialOverflowBuffer->begin());
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(itr == partialOverflowBuffer->begin());
		EXPECT_TRUE(itr != partialOverflowBuffer->begin());

		EXPECT_FALSE(itr < partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr <= partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr > partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(itr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(itr != partialOverflowBuffer->begin() + 5);

		EXPECT_FALSE(itr < partialOverflowBuffer->end());
		EXPECT_TRUE(itr <= partialOverflowBuffer->end());
		EXPECT_FALSE(itr > partialOverflowBuffer->end());
		EXPECT_TRUE(itr >= partialOverflowBuffer->end());
		EXPECT_TRUE(itr == partialOverflowBuffer->end());
		EXPECT_FALSE(itr != partialOverflowBuffer->end());

		// const
		EXPECT_FALSE(citr < partialOverflowBuffer->begin());
		EXPECT_TRUE(citr <= partialOverflowBuffer->begin());
		EXPECT_FALSE(citr > partialOverflowBuffer->begin());
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin());
		EXPECT_TRUE(citr == partialOverflowBuffer->begin());
		EXPECT_FALSE(citr != partialOverflowBuffer->begin());

		EXPECT_TRUE(citr < partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr <= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr > partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(citr < partialOverflowBuffer->end());
		EXPECT_TRUE(citr <= partialOverflowBuffer->end());
		EXPECT_FALSE(citr > partialOverflowBuffer->end());
		EXPECT_FALSE(citr >= partialOverflowBuffer->end());
		EXPECT_FALSE(citr == partialOverflowBuffer->end());
		EXPECT_TRUE(citr != partialOverflowBuffer->end());

		citr += 4;

		EXPECT_FALSE(citr < partialOverflowBuffer->begin());
		EXPECT_FALSE(citr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(citr > partialOverflowBuffer->begin());
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(citr == partialOverflowBuffer->begin());
		EXPECT_TRUE(citr != partialOverflowBuffer->begin());

		EXPECT_TRUE(citr < partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr <= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr > partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(citr < partialOverflowBuffer->end());
		EXPECT_TRUE(citr <= partialOverflowBuffer->end());
		EXPECT_FALSE(citr > partialOverflowBuffer->end());
		EXPECT_FALSE(citr >= partialOverflowBuffer->end());
		EXPECT_FALSE(citr == partialOverflowBuffer->end());
		EXPECT_TRUE(citr != partialOverflowBuffer->end());

		++citr;

		EXPECT_FALSE(citr < partialOverflowBuffer->begin());
		EXPECT_FALSE(citr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(citr > partialOverflowBuffer->begin());
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(citr == partialOverflowBuffer->begin());
		EXPECT_TRUE(citr != partialOverflowBuffer->begin());

		EXPECT_FALSE(citr < partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr <= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr > partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr == partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(citr < partialOverflowBuffer->end());
		EXPECT_TRUE(citr <= partialOverflowBuffer->end());
		EXPECT_FALSE(citr > partialOverflowBuffer->end());
		EXPECT_FALSE(citr >= partialOverflowBuffer->end());
		EXPECT_FALSE(citr == partialOverflowBuffer->end());
		EXPECT_TRUE(citr != partialOverflowBuffer->end());

		++citr;

		EXPECT_FALSE(citr < partialOverflowBuffer->begin());
		EXPECT_FALSE(citr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(citr > partialOverflowBuffer->begin());
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(citr == partialOverflowBuffer->begin());
		EXPECT_TRUE(citr != partialOverflowBuffer->begin());

		EXPECT_FALSE(citr < partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr <= partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr > partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr != partialOverflowBuffer->begin() + 5);

		EXPECT_TRUE(citr < partialOverflowBuffer->end());
		EXPECT_TRUE(citr <= partialOverflowBuffer->end());
		EXPECT_FALSE(citr > partialOverflowBuffer->end());
		EXPECT_FALSE(citr >= partialOverflowBuffer->end());
		EXPECT_FALSE(citr == partialOverflowBuffer->end());
		EXPECT_TRUE(citr != partialOverflowBuffer->end());

		citr = partialOverflowBuffer->cend();

		EXPECT_FALSE(citr < partialOverflowBuffer->begin());
		EXPECT_FALSE(citr <= partialOverflowBuffer->begin());
		EXPECT_TRUE(citr > partialOverflowBuffer->begin());
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin());
		EXPECT_FALSE(citr == partialOverflowBuffer->begin());
		EXPECT_TRUE(citr != partialOverflowBuffer->begin());

		EXPECT_FALSE(citr < partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr <= partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr > partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr >= partialOverflowBuffer->begin() + 5);
		EXPECT_FALSE(citr == partialOverflowBuffer->begin() + 5);
		EXPECT_TRUE(citr != partialOverflowBuffer->begin() + 5);

		EXPECT_FALSE(citr < partialOverflowBuffer->end());
		EXPECT_TRUE(citr <= partialOverflowBuffer->end());
		EXPECT_FALSE(citr > partialOverflowBuffer->end());
		EXPECT_TRUE(citr >= partialOverflowBuffer->end());
		EXPECT_TRUE(citr == partialOverflowBuffer->end());
		EXPECT_FALSE(citr != partialOverflowBuffer->end());
	}

	TEST_F(circular_queue_test, TimeTrial)
	{
		test_object::clearfunctioncalls();
		EXPECT_EQ(0, test_object::getconstructorcalls());
		{
			std::chrono::high_resolution_clock::time_point vbegin, vend, qbegin, qend;
			std::chrono::duration<double>                  vdiff, qdiff;
			size_t                                         numElements = 10000;
			size_t                                         capacity    = 1024;

			//////////////////////////////////////////////////////////////////////////
			//		CONSTRUCT/DESTRUCT TEST
			//////////////////////////////////////////////////////////////////////////
			{
				std::cout << capacity << " Queue / " << capacity << " Vector: \n";

				vbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					std::vector<test_object> vec;
					vec.reserve(capacity);
				}
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					circular_queue<test_object> queue(capacity);
				}
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "vector construct/destruct time: " << vdiff.count() << "s\n";
				std::cout << "queue construct/destruct time:  " << qdiff.count() << "s\n";
			}

			//////////////////////////////////////////////////////////////////////////
			//		PUSH TEST
			//////////////////////////////////////////////////////////////////////////
			{
				std::vector<test_object> vec;
				vec.reserve(capacity);
				circular_queue<test_object> queue(capacity);

				std::cout << queue.capacity() << " Queue / " << vec.capacity() << " Vector: \n";

				vbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					vec.push_back('x');
				}
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					queue.push_back('x');
				}
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "vector push time: " << vdiff.count() << "s\n";
				std::cout << "queue push time:  " << qdiff.count() << "s\n";
			}

			//////////////////////////////////////////////////////////////////////////
			//		EMPLACE TEST
			//////////////////////////////////////////////////////////////////////////
			{

				std::vector<test_object> vec;
				vec.reserve(capacity);
				circular_queue<test_object> queue(capacity);

				std::cout << queue.capacity() << " Queue / " << vec.capacity() << " Vector: \n";

				vbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					vec.emplace_back('x');
				}
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					queue.emplace_back('x');
				}
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "vector emplace time: " << vdiff.count() << "s\n";
				std::cout << "queue emplace time:  " << qdiff.count() << "s\n";
			}

			//////////////////////////////////////////////////////////////////////////
			//		INSERT TEST
			//////////////////////////////////////////////////////////////////////////
			{

				std::vector<test_object> vec;
				vec.reserve(capacity);
				circular_queue<test_object> queue(capacity);

				std::cout << queue.capacity() << " Queue / " << vec.capacity() << " Vector: \n";

				for (int i = 0; i < capacity; ++i)
				{
					vec.emplace_back('x');
				}

				for (int i = 0; i < capacity; ++i)
				{
					queue.emplace_back('x');
				}

				vbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements / 100; ++i)
				{
					vec.insert(vec.begin() + capacity / 2, 'z');
				}
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements / 100; ++i)
				{
					queue.insert(queue.begin() + capacity / 2, 'z');
				}
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "vector insert time: " << vdiff.count() << "s\n";
				std::cout << "queue insert time:  " << qdiff.count() << "s\n";
			}

			//////////////////////////////////////////////////////////////////////////
			//		POP TEST
			//////////////////////////////////////////////////////////////////////////
			{

				std::vector<test_object> vec;
				vec.reserve(capacity);
				circular_queue<test_object> queue(capacity);

				std::cout << queue.capacity() << " Queue / " << vec.capacity() << " Vector: \n";

				for (int i = 0; i < capacity; ++i)
				{
					vec.emplace_back('x');
				}

				for (int i = 0; i < capacity; ++i)
				{
					queue.emplace_back('x');
				}

				vbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < capacity; ++i)
				{
					vec.pop_back();
				}
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < capacity; ++i)
				{
					queue.pop_back();
				}
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "vector pop time: " << vdiff.count() << "s\n";
				std::cout << "queue pop time:  " << qdiff.count() << "s\n";
			}

			//////////////////////////////////////////////////////////////////////////
			//		POWER OF TWO TEST
			//////////////////////////////////////////////////////////////////////////
			{

				circular_queue<test_object> queue(803);
				circular_queue<test_object> queue2(1024);

				std::cout << queue.capacity() << " Queue / " << queue2.capacity() << " Queue: \n";

				vbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					queue.emplace_back('x');
				}
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				for (int i = 0; i < numElements; ++i)
				{
					queue2.emplace_back('x');
				}
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "queue pop time: " << vdiff.count() << "s\n";
				std::cout << "queue (2) pop time:  " << qdiff.count() << "s\n";
			}

			//////////////////////////////////////////////////////////////////////////
			//		COPY CONSTRUCT VS. MEMCPY
			//////////////////////////////////////////////////////////////////////////
			{

				circular_queue<int>    queue(500000, 5);
				std::unique_ptr<int[]> output(new int[500000]);

				vbegin = std::chrono::high_resolution_clock::now();
				queue.copy(queue.begin(), queue.end(), &output[0], &memcpy);
				vend  = std::chrono::high_resolution_clock::now();
				vdiff = vend - vbegin;

				qbegin = std::chrono::high_resolution_clock::now();
				circular_queue<int> queue2(queue);
				qend  = std::chrono::high_resolution_clock::now();
				qdiff = qend - qbegin;

				std::cout << "memcpy time: " << vdiff.count() << "s\n";
				std::cout << "copy constructor time:  " << qdiff.count() << "s\n";
			}
		}
	}
}    // namespace
