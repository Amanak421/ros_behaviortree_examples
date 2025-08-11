cmake_minimum_required(VERSION 3.5)
project(simple_server_client)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(behaviortree_ros2 REQUIRED)
find_package(rosidl_default_runtime REQUIRED)

# Generate action interface
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/Fibonacci.action"
  DEPENDENCIES builtin_interfaces
)

# Create the shared libraries
add_library(action_server SHARED
  src/server.cpp
)

add_library(simple_bt_node SHARED
  src/bt_ros_node.cpp
)

target_include_directories(action_server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(simple_bt_node PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(simple_bt_node PRIVATE "MY_COMPOSITION_BUILDING_DLL")

ament_target_dependencies(action_server
  rclcpp
  rclcpp_action
  rclcpp_components
  std_msgs
  ament_index_cpp
  behaviortree_ros2
)

ament_target_dependencies(simple_bt_node
  behaviortree_ros2
  behaviortree_cpp
  ament_index_cpp
  rclcpp
  rclcpp_components
  rclcpp_action
  std_msgs
)

rosidl_get_typesupport_target(
  cpp_typesupport_target     
  ${PROJECT_NAME}            
  "rosidl_typesupport_cpp"   
)

target_link_libraries(action_server
  "${cpp_typesupport_target}"
)

target_link_libraries(simple_bt_node
  "${cpp_typesupport_target}"
)


rclcpp_components_register_nodes(simple_bt_node
  "example_simple_client_node::SimpleBTNode"
)

rclcpp_components_register_node(action_server
  PLUGIN "simple_action_server::FibonacciActionServer"
  EXECUTABLE fibonacci_action_server
)

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
