from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():

    return LaunchDescription([
        ComposableNodeContainer(
            name='simple_subscriber_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='simple_report_node',
                    plugin='example_simple_report_subscriber::SimpleSubscriber',
                    name='SimpleSubscriber'
                )
            ],
            output='screen',
        )
    ])
