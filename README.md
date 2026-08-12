![Build Status](https://github.com/nholthaus/queue/actions/workflows/cmake-multi-platform.yml/badge.svg) ![language](https://img.shields.io/badge/language-c++23-blue.svg) ![license](https://img.shields.io/badge/license-MIT-orange.svg) ![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg) ![msvc2022](https://img.shields.io/badge/MSVC-2022-ff69b4.svg) ![gcc-13](https://img.shields.io/badge/GCC-13-ff69b4.svg) ![clang-18](https://img.shields.io/badge/Clang-18-ff69b4.svg)

A modern C++ header-only library of various types of queue

- `concurrent_queue` provides synchronized FIFO access for multiple producers and consumers. In addition to `push` and `emplace`, consumers can use non-blocking `try_pop` or bounded-wait `try_pop_for` operations.
- `circular_queue` is an allocator-aware, fixed-capacity contiguous container with random-access iterators. When full, insertion overwrites the oldest element at the opposite end.

# Table of Contents

- [Table of Contents](#table-of-contents)
- [Compatibility](#compatibility)
- [Release 1.2.0](#release-120)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
  - [Concurrent Queue](#concurrent-queue)
    - [Example](#example)
  - [Circular Queue](#circular-queue)
    - [Example](#example-1)

# Compatibility

Release 1.2.0 requires C++23. The versions below are the minimum compiler baselines exercised by each release's CI or explicitly documented by that release; newer compiler releases are expected to work as well.

| Release | Language standard | Minimum tested compilers | Minimum CMake |
| --- | --- | --- | --- |
| 1.2.0 | C++23 | GCC 13, Clang 18, MSVC 2022 | 3.25 |
| 1.1.x | C++20 | GCC 11.4, Clang 14, MSVC 2022 | 3.7 |
| 1.0.x | C++17 | GCC 9.3, MSVC 2019 | 3.7 |

Compiler versions not listed for an older release were not pinned by that release's build configuration and are therefore not part of its documented compatibility baseline. Other compilers may work, but are not supported unless covered by CI.

# Release 1.2.0

The 1.2.0 release moves the library to C++23 and adds range-aware construction, `assign_range`, and `insert_range` to `circular_queue`. Both queue types support three-way comparison when their stored types and underlying containers support it. This release also expands the GCC, Clang, and MSVC CI matrix with coverage and sanitizer builds.

# Build Instructions

Each class in the library itself is a single header only. You can use the included CMake in a subdirectory and add the interface library, or just copy the `*.h` files (and license) into your project.

Building the repository requires CMake 3.25 or newer and a C++23 compiler from the 1.2.0 compatibility row above. The queue headers themselves have no third-party runtime dependency. Tests require a thread library and GoogleTest 1.17 (fetched automatically when it is not installed); documentation generation requires Doxygen, with `pdflatex` needed only for the optional PDF.

When the repository is included with `add_subdirectory`, link the header-only interface target to inherit its include path:

``` cmake
add_subdirectory(queue)
target_link_libraries(my_target PRIVATE queue)
```

To build and run the unit tests:

``` bash
cmake -S . -B build -DQUEUE_BUILD_TESTS=ON -DQUEUE_BUILD_DOCUMENTATION=OFF
cmake --build build --config Release
ctest --test-dir build --build-config Release --output-on-failure
```

To configure and build the generated documentation:

``` bash
cmake -S . -B build -DQUEUE_BUILD_TESTS=OFF -DQUEUE_BUILD_DOCUMENTATION=ON
cmake --build build --target queue_documentation
```

# Usage

## Concurrent Queue

The `concurrent_queue` class is a templated FIFO (first-in, first-out) queue that is suitable for concurrent use, including cases where there are multiple producers and multiple consumers reading and writing the queue simultaneously. It has an atomic `push`/`pop` interface for enqueueing and dequeueing elements.

### Example

Run it live on [Compiler Explorer](https://godbolt.org/z/Gn3znj).

``` cpp
#include <chrono>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

#include <concurrent_queue.h>

using namespace std::chrono_literals;

int main()
{
        // Push 100 ints from one deque to another via a set of producer/consumer threads
        // running at different rates via a concurrent queue
        
        constexpr int    test_size = 100;
        std::deque<int> numbers_in(test_size);
        std::deque<int> numbers_out;
        std::iota(numbers_in.begin(), numbers_in.end(), 0);
        
        std::cout << "In size: " << numbers_in.size() << std::endl;
        std::cout << "Out size: " << numbers_out.size() << std::endl;
        
        concurrent_queue<int> queue;
        
        // produce integers at 200Hz and consume them at 1000 Hz
        auto producer = std::async(std::launch::async, [&queue, &numbers_in]
               {
                   while(!numbers_in.empty())
                   {
                       queue.push(numbers_in.front());
                       numbers_in.pop_front();
                       std::this_thread::sleep_for(5ms);
                   }
                   return std::this_thread::get_id();
               });
        auto consumer = std::async(std::launch::async, [&queue, &numbers_out, test_size]
               {
                   int val;
                   while (numbers_out.size() < test_size)
                   {
                       if (queue.try_pop_for(val, 1ms))
                           numbers_out.push_back(val);
                   }
                   return std::this_thread::get_id();
               });

        auto producer_id = producer.get();
        auto consumer_id = consumer.get();
        
        std::cout << "In size: " << numbers_in.size() << std::endl;
        std::cout << "Out size: " << numbers_out.size() << std::endl;
}
```

## Circular Queue

The `circular_queue` class is a fixed capacity, STL-style, templated circular buffer.

See http://en.wikipedia.org/wiki/Circular_buffer for details of how circular buffers work.

This implementation shares many features of a double-ended queue. This structure allows for the individual elements to be accessed directly through random access iterators, with storage handled automatically by over-writing elements from the opposite end of the container as it grows. As long as elements are added consistently to EITHER the front or the back, once full, the container will expand by over-writing the oldest element. If elements are added to both sides of the buffer, the behavior is effectively user defined.

Therefore, the `circular_queue` provides a functionality similar to vectors, but with efficient insertion and deletion of elements also at the beginning of the sequence, and not only at its end. Also, like vectors (and unlike dequeues), all elements are stored in contiguous memory locations, and offset access IS allowed.

Both vectors and the `circular_queue` provide a very similar interface and can be used for similar purposes, but internally both work in quite different ways: While vectors use a single array that needs to be occasionally reallocated for growth, the elements of a `circular_queue` are over-written once the buffer reaches its maximum capacity, ensuring elements are inserted and accessed in constant time and with a uniform sequential interface (through iterators), as opposed to the vector's amortized constant time. This allows them to grow more efficiently under certain circumstances, especially with very long sequences, where reallocation's become more expensive, and real-time applications where stale data is not as useful as current data.

For operations that involve frequent insertion or removals of elements at positions other than the beginning or the end, `circular_queue` performs worse and has less consistent iterators and references than lists and forward lists.

### Example

Run it live on [Compiler Explorer](https://godbolt.org/z/f3dscW).

``` cpp
#include <iostream>
#include <circular_queue.h>

int main()
{
    circular_queue<int> q(10);

    for(int i = 0; i < 13; ++i)
    {
        q.push_back(i);
        if(q.full()) std::cout << "Buffer Overflow!" << std::endl;
    }

    for(unsigned int i = 0; i < q.size(); ++i)
    {
        std::cout << "q[" << i << "]: " << q[i] << std::endl;
    }
}

```
