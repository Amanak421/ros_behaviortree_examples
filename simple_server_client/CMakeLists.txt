cmake_minimum_required(VERSION 3.5)
project(simple_server_client)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(mrs_lib REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(behaviortree_ros2 REQUIRED)
find_package(rosidl_default_runtime REQUIRED)

# Generate action interface
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/Fibonacci.action"
  DEPENDENCIES builtin_interfaces
)

# Create the shared library for action server
add_library(action_server SHARED
  src/server.cpp
)

# Create the shared library for BT node
add_library(simple_bt_node SHARED
  src/bt_ros_node.cpp
)

# Include headers
target_include_directories(action_server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(simple_bt_node PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(simple_bt_node PRIVATE "MY_COMPOSITION_BUILDING_DLL")

# Link dependencies for action_server (including generated action)
ament_target_dependencies(action_server
  rclcpp
  rclcpp_action
  rclcpp_components
  std_msgs
  behaviortree_cpp
  mrs_lib
  ament_index_cpp
  Eigen3
  behaviortree_ros2
)

rosidl_target_interfaces(action_server
  ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

# Link dependencies for simple_bt_node (including generated action)
ament_target_dependencies(simple_bt_node
  behaviortree_ros2
  behaviortree_cpp
  ament_index_cpp
  rclcpp
  rclcpp_components
  rclcpp_action
  std_msgs
)

rosidl_target_interfaces(simple_bt_node
  ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

# Register components
rclcpp_components_register_nodes(simple_bt_node
  "example_simple_client_node::SimpleBTNode"
)

rclcpp_components_register_node(action_server
  PLUGIN "simple_action_server::FibonacciActionServer"
  EXECUTABLE fibonacci_action_server
)

# Install targets
install(TARGETS
  action_server
  simple_bt_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Install launch and config folders
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY behavior_tree/
  DESTINATION share/${PROJECT_NAME}/behavior_tree
)

ament_package()
