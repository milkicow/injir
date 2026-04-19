add_executable(constant_folding_test constant_folding.cpp)
add_executable(peephole_test peephole.cpp)
add_executable(inlining_test inline.cpp)
add_executable(check_elimination_test check_elimination.cpp)

target_link_libraries(constant_folding_test PRIVATE injir GTest::gtest_main)
target_link_libraries(peephole_test PRIVATE injir GTest::gtest_main)
target_link_libraries(inlining_test PRIVATE injir GTest::gtest_main)
target_link_libraries(check_elimination_test PRIVATE injir GTest::gtest_main)
