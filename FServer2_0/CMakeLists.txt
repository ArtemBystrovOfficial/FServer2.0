#cmake_minimum_required("3.12")

#project(FServer2.0)

cmake_minimum_required(VERSION 3.2)

if(ON_TESTS)

# Build googletest
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest" 
    "lib/sgoogletest"
)

    # Test target
    add_executable (tests_client "testing/tests_client.cpp")
    add_executable (tests_server "testing/tests_server.cpp")
    add_executable (tests_connection "testing/tests_connection.cpp")

    target_link_libraries(
            tests_server
            PRIVATE libFS
    )


    target_link_libraries(
            tests_client
            PRIVATE libFC
    )

    target_link_libraries(
        tests_connection 
            PRIVATE libFS libFC
    )


    # Link test target against gtest libraries
    target_link_libraries(
        tests_client PRIVATE
        gtest gtest_main
        gmock gmock_main
    )

    target_link_libraries(
        tests_server PRIVATE
        gtest gtest_main
        gmock gmock_main
    )

    target_link_libraries(
        tests_connection PRIVATE
        gtest gtest_main
        gmock gmock_main
    )


    if (MSVC)
        # @see https://github.com/google/googletest/blob/master/googletest/README.md
        # Visual Studio Dynamic vs Static Runtimes
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        
        target_compile_options(
            tests_client PRIVATE
            /W4
        )
        target_compile_options(
            tests_server PRIVATE
            /W4
        )
        target_compile_options(
            tests_connection PRIVATE
            /W4
        )

    else ()

        #run time library fix google-test

        if(WIN32)

        set(CMAKE_CXX_FLAGS_RELEASE "/MT")
        set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

        endif()

        target_compile_options(
            tests_client PRIVATE
            -Wall -Wextra -pedantic 
        )
        target_compile_options(
            tests_server PRIVATE
            -Wall -Wextra -pedantic 
        )
        target_compile_options(
            tests_connection PRIVATE
            -Wall -Wextra -pedantic 
        )
        
    endif()

endif()


#target_link_libraries (tests_client PRIVATE ../build/lib/Debug/gtest.lib ../build/lib/Debug/gtest_main.lib)
#target_link_libraries (tests_server PRIVATE ../build/lib/Debug/gtest.lib ../build/lib/Debug/gtest_main.lib)

#target_link_directories(tests_client PRIVATE "../build/lib/Debug")
#target_link_directories(tests_server PRIVATE "../build/lib/Debug")

#target_include_directories(tests_client PRIVATE "libs/googletest/googletest")
#target_include_directories(tests_server PRIVATE "libs/googletest/googletest")

add_subdirectory(source)
