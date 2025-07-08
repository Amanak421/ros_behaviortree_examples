cmake_minimum_required(VERSION 3.10)
project(ros_bt_examples)

# -------------------- compiler settings --------------------
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the compile options to show code warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CATKIN_DEPS
  nodelet
  roscpp
  std_msgs
  mrs_lib
  behaviortree_cpp
)

find_package(catkin REQUIRED COMPONENTS
  ${CATKIN_DEPS}
)

# -------------------- package information --------------------
#
# catkin_package() populates <pkg>Config.cmake so that other ROS packages
# can `find_package(ros_bt_examples)` and receive:
#   * include directories
#   * libraries to link against
#   * runtime dependencies
#
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES t_sub_pub_example_nodelet
  CATKIN_DEPENDS ${CATKIN_DEPS}
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

# ------------------- libraries --------------------

add_library(t_sub_pub_example_nodelet
  src/nodelet_sub_pub.cpp
  src/bt_nodes.cpp
  )

add_dependencies(t_sub_pub_example_nodelet
  ${catkin_EXPORTED_TARGETS}
)

target_link_libraries(t_sub_pub_example_nodelet
  ${catkin_LIBRARIES}
)

target_include_directories(t_sub_pub_example_nodelet PUBLIC
  ${catkin_INCLUDE_DIRS}
)

# -------------------- installation --------------------

install(TARGETS t_sub_pub_example_nodelet
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(DIRECTORY config
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(FILES plugins.xml behavior.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  )