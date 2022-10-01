project(asioPRACTICE)

include_directories("FServer2_0/source/Client")
include_directories("FServer2_0/source/Server")
include_directories("FServer2_0/libs/asio-1.24.0/include")



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(FServer FServer.cpp)
add_executable(FClient FClient.cpp)

target_compile_features(FServer PRIVATE cxx_std_17)
target_compile_features(FClient PRIVATE cxx_std_17)



add_subdirectory(FServer2_0)

target_link_libraries(FServer PRIVATE libFS)
target_link_libraries(FClient PRIVATE libFC)


set(CMAKE_CXX_FLAGS "-pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
