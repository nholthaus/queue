![Queue](resources/queue_banner_transparent.png)

[![Build Status](https://travis-ci.org/nholthaus/queue.svg?branch=main)](https://travis-ci.org/github/nholthaus/queue) [![Build status](https://ci.appveyor.com/api/projects/status/dfoks5b44qsgf7ka/branch/master?svg=true)](https://ci.appveyor.com/project/nholthaus/queue/branch/master) ![language](https://img.shields.io/badge/language-c++17-blue.svg) ![license](https://img.shields.io/badge/license-MIT-orange.svg) ![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg) ![msvc2019](https://img.shields.io/badge/MSVC-2019-ff69b4.svg) ![gcc-9.3.0](https://img.shields.io/badge/GCC-9.3.0-ff69b4.svg)

A modern C++ header-only library of various types of queue

# Table of Contents
<!-- TOC -->

- [Concurrent Queue](#concurrent-queue)
    - [Example](#example)
- [Circular Queue](#circular-queue)
    - [Example](#example-1)

<!-- /TOC -->

# Build Instructions

Each class in the library itself is a single header only. You can use the included CMake in a subdirectory and add the interface library, or just copy the `*.h` files (and license) into your project.

To build and run the unit tests:

``` bash
mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
cmake --build . --config Release
ctest
```

To build the documentation, replace the above `cmake` command with the following:

```
cmake -DBUILD_TESTS=ON -DBUILD_DOCUMENTATION=ON ..
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
