add_executable(loop_test loop.cpp)
add_executable(lifetime_test lifetime.cpp)

target_include_directories(loop_test PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_include_directories(lifetime_test PRIVATE ${CMAKE_SOURCE_DIR}/tests)

target_link_libraries(loop_test PRIVATE injir GTest::gtest_main)
target_link_libraries(lifetime_test PRIVATE injir GTest::gtest_main)
