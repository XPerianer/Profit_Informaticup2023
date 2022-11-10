include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(
    test_NullObject
    src/test_NullObject.cpp
  )

  target_link_libraries(
    test_NullObject
    GTest::gtest_main
  )
  
  include(GoogleTest)
  gtest_discover_tests(test_NullObject)
