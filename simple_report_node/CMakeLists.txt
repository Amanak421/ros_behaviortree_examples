cmake_minimum_required(VERSION 3.5)
project(simple_report_node)

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

find_package(rosidl_default_generators REQUIRED)

set(msg_files
  "msg/Status.msg"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  DEPENDENCIES std_msgs
)

ament_export_dependencies(rosidl_default_runtime)

add_library(simple_bt_node SHARED
  src/bt_ros_node.cpp
)

add_library(subscriber SHARED
  src/subscriber.cpp
)


target_compile_definitions(simple_bt_node PRIVATE "MY_COMPOSITION_BUILDING_DLL")
ament_target_dependencies(simple_bt_node mrs_lib behaviortree_cpp ament_index_cpp Eigen3 rclcpp rclcpp_components std_msgs)
target_include_directories(simple_bt_node PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_definitions(subscriber PRIVATE "MY_COMPOSITION_BUILDING_DLL")
ament_target_dependencies(subscriber rclcpp rclcpp_components std_msgs)

rclcpp_components_register_nodes(simple_bt_node "example_simple_report_node::SimpleBTNode")
rclcpp_components_register_nodes(subscriber "example_simple_report_subscriber::SimpleSubscriber")

rosidl_target_interfaces(simple_bt_node
  ${PROJECT_NAME} "rosidl_typesupport_cpp")

rosidl_target_interfaces(subscriber
  ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS simple_bt_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(TARGETS subscriber
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
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