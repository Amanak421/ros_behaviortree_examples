cmake_minimum_required(VERSION 3.5)
project(ros_behaviortree_examples)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)

add_library(publisher SHARED
  src/publisher.cpp
)

target_compile_definitions(publisher PRIVATE "MY_COMPOSITION_BUILDING_DLL")
ament_target_dependencies(publisher rclcpp rclcpp_components std_msgs)

rclcpp_components_register_nodes(publisher "example_simple_publisher::SimplePublisher")

install(TARGETS publisher
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()