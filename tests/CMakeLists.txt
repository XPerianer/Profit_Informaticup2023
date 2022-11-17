include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(
  unit_test
  test_serialization.cpp
)

target_link_libraries(
  unit_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_test)
