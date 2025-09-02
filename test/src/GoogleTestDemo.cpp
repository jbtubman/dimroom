// Test driver.
#include "gtest/gtest.h"
#include "../include/google_test_fixture.hpp"
#include "../include/cell_test.hpp"
#include "../include/cell_types_test.hpp"
#include "../include/command_interpreter_test.hpp"
#include "../include/coordinates_test.hpp"
#include "../include/parse_utils_test.hpp"
#include "../include/parser_test.hpp"
#include "../include/query_test.hpp"
#include "../include/table_test.hpp"
#include "../include/utility_test.hpp"

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
