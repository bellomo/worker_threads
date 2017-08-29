set(sources
        run_unit_gtests.cpp
        test_worker.cpp
        )

add_executable(tests_worker ${sources})
message("GMOCK_INCLUDE_DIR ${GTEST_INCLUDE_DIR}")
target_link_libraries(tests_worker PRIVATE worker GTest::GTest)
target_compile_definitions(tests_worker PUBLIC GTEST_LINKED_AS_SHARED_LIBRARY=1)
