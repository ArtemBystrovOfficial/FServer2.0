project(asioPRACTICE)

include_directories("FServer2_0/source/Client")
include_directories("FServer2_0/source/Server")
include_directories("FServer2_0/libs/asio-1.24.0/include")


set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)
add_subdirectory(FServer2_0)

target_link_libraries(main PRIVATE libFS)
set(CMAKE_CXX_FLAGS "-pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)