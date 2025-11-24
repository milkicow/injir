add_executable(constant_folding_test constant_folding.cpp)
add_executable(peephole_test peephole.cpp)

target_link_libraries(constant_folding_test PRIVATE injir GTest::gtest_main)
target_link_libraries(peephole_test PRIVATE injir GTest::gtest_main)
