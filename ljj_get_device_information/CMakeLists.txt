cmake_minimum_required(VERSION 3.1.0)

project(ljj_get_device_information)

# Find librealsense2 installed package
find_package(realsense2 REQUIRED)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Add the application sources to the target
add_executable(${PROJECT_NAME} test.cpp)

# Link librealsense2 to the target
target_link_libraries(${PROJECT_NAME} ${realsense2_LIBRARY})
