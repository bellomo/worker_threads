set(BUILD_TEST_TARGETS ON)
if (BUILD_TEST_TARGETS)
    find_package(GTest REQUIRED)
endif()