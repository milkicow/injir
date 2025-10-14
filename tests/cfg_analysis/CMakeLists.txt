add_executable(dfs_test dfs.cpp)
add_executable(rpo_test rpo.cpp)
add_executable(dom_test dom.cpp)

target_link_libraries(dfs_test PRIVATE injir GTest::gtest_main)
target_link_libraries(rpo_test PRIVATE injir GTest::gtest_main)
target_link_libraries(dom_test PRIVATE injir GTest::gtest_main)
