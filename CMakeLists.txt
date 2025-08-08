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
find_package(behaviortree_cpp REQUIRED)
find_package(mrs_lib REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(Eigen3 REQUIRED)

add_library(publisher SHARED
  src/publisher.cpp
)

add_library(simple_bt_node SHARED
  src/bt_ros_node.cpp
)

target_compile_definitions(publisher PRIVATE "MY_COMPOSITION_BUILDING_DLL")
ament_target_dependencies(publisher rclcpp rclcpp_components std_msgs)

target_compile_definitions(simple_bt_node PRIVATE "MY_COMPOSITION_BUILDING_DLL")
ament_target_dependencies(simple_bt_node mrs_lib behaviortree_cpp ament_index_cpp Eigen3 rclcpp rclcpp_components std_msgs)
target_include_directories(simple_bt_node PRIVATE ${PROJECT_SOURCE_DIR}/include)

rclcpp_components_register_nodes(publisher "example_simple_publisher::SimplePublisher")
rclcpp_components_register_nodes(simple_bt_node "example_simple_ros_node::SimpleBTNode")

install(TARGETS publisher
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS simple_bt_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY behavior_tree/
  DESTINATION share/${PROJECT_NAME}/behavior_tree)

ament_package()