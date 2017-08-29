/// @brief Main file for the unit test executable

#include <gtest/gtest.h>

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#endif

#include <iostream>

int main(int argc, char** argv)
{
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif
    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    std::cout << "Tests finished with return value: " << ret << std::endl;

    return EXIT_SUCCESS;
}