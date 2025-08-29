// Trying out Google Test.
#include "gtest/gtest.h"
#include "../include/google_test_fixture.hpp"
#include "../include/cell_types_test.hpp"

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
