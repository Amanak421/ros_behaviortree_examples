#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

using namespace std::chrono_literals;

namespace example_simple_publisher
{
    class SimplePublisher : public rclcpp::Node
    {
    public:
        SimplePublisher(const rclcpp::NodeOptions& options);
    private:
        /* is set to true when the nodelet is initialized, useful for rejecting callbacks that are called before the node is initialized */
        std::atomic<bool> is_initialized_ = false;

        /* simple string publisher */
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
        void timer_callback();

        int count_ = 0;

        rclcpp::TimerBase::SharedPtr timer_initializer_;
        void initialize();
    };

    SimplePublisher::SimplePublisher(const rclcpp::NodeOptions& options): Node("simple_publisher", options)
    {
        timer_initializer_ = create_wall_timer(std::chrono::duration<double>(1.0), std::bind(&SimplePublisher::initialize,this));
    }

    void SimplePublisher::initialize(){

        publisher_ = this->create_publisher<std_msgs::msg::String>("string_counter", 10);
        timer_ = this->create_wall_timer(1s, std::bind(&SimplePublisher::timer_callback,this));

        is_initialized_ = true;
        timer_initializer_->cancel();
    }

    void SimplePublisher::timer_callback(){
        auto message = std_msgs::msg::String();
        message.data = "Actual count: " + std::to_string(this->count_++);
        // print debug message
        RCLCPP_INFO(this->get_logger(), "[SimplePublisher] Publishing: '%s'", message.data.c_str());
        this->publisher_->publish(message);
    }
}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(example_simple_publisher::SimplePublisher);