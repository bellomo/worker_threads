set(headers include/worker/worker.h
            include/worker/acout.h
            include/worker/exceptions.h
        )

set(sources src/worker.cpp
        )

add_library(worker ${sources} ${headers})
target_include_directories(worker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (BUILD_TEST_TARGETS)
    add_subdirectory(tests)
endif()
