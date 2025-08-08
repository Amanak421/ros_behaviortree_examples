from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    config_file = os.path.join(get_package_share_directory('simple_sub_pub'), 'config', 'simple_bt_node.yaml')

    return LaunchDescription([
        ComposableNodeContainer(
            name='simple_bt_node_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',  # Multithreaded container
            composable_node_descriptions=[
                ComposableNode(
                    package='simple_sub_pub',
                    plugin='example_simple_ros_node::SimpleBTNode',
                    name='SimpleBTNode',
                    parameters=[config_file]
                )
            ],
            output='screen',
        )
    ])