#include "test_object.h"
#include "circular_queue_test.h"
#include "concurrent_queue_test.h"
#include <gtest/gtest.h>
#include <type_traits>

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
