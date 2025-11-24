add_executable(factorial_test factorial.cpp)

target_link_libraries(factorial_test PRIVATE injir GTest::gtest_main)

add_subdirectory(cfg_analysis)
add_subdirectory(pass)
