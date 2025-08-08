from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    return LaunchDescription([
        ComposableNodeContainer(
            name='simple_bt_node_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',  # Multithreaded container
            composable_node_descriptions=[
                ComposableNode(
                    package='ros_behaviortree_examples',
                    plugin='example_simple_ros_node::SimpleBTNode',
                    name='SimpleBTNode'
                )
            ],
            output='screen',
        )
    ])