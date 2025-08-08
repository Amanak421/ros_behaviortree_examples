#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

using namespace std::chrono_literals;

namespace example_simple_subscriber
{
    class SimpleSubscriber : public rclcpp::Node
    {
    public:
        SimpleSubscriber(const rclcpp::NodeOptions& options);

    private:
        /* is set to true when the nodelet is initialized, useful for rejecting callbacks that are called before the node is initialized */
        std::atomic<bool> is_initialized_ = false;

        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

        rclcpp::TimerBase::SharedPtr timer_initializer_;
        void initialize();
    };

    SimpleSubscriber::SimpleSubscriber(const rclcpp::NodeOptions& options): Node("simple_subscriber", options)
    {
        timer_initializer_ = create_wall_timer(std::chrono::duration<double>(1.0), std::bind(&SimpleSubscriber::initialize,this));
    }

    void SimpleSubscriber::initialize(){
        /* Inspired from ros2 jazzy example */
        auto topic_callback =
            [this](std_msgs::msg::String::UniquePtr msg) -> void {
                RCLCPP_INFO(this->get_logger(), "Received: '%s'", msg->data.c_str());
            };
        subscription_ =
            this->create_subscription<std_msgs::msg::String>("bt_output", 10, topic_callback);

        is_initialized_ = true;
        timer_initializer_->cancel();
    }
}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(example_simple_subscriber::SimpleSubscriber);