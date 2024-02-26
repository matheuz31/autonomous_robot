cmake_minimum_required(VERSION 3.5)
project(package_1)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# Declare a C++ executable
add_executable(talker src/robot_talker.cpp)

# Specify libraries to link a library or executable target against
ament_target_dependencies(talker
  rclcpp
  std_msgs
)

# Install the executable
install(TARGETS
  talker
  DESTINATION lib/${PROJECT_NAME}/
)

ament_package()
