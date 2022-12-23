include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# We have a custom FAIL macro. Use GTEST_FAIL to refer to the google macro
add_definitions(-DGTEST_DONT_DEFINE_FAIL)

add_compile_options(${COMPILER_WARNING_FLAGS})

add_executable(
  unit_test
  mdkp_test.cpp
  serialization_test.cpp
  parsing_test.cpp
  geometry_test.cpp
  distance_map_test.cpp
  occupancy_map_test.cpp
  connected_component_test.cpp
)

target_link_libraries(
  unit_test
  GTest::gtest_main
  gmock
)

include(GoogleTest)
gtest_discover_tests(unit_test)
