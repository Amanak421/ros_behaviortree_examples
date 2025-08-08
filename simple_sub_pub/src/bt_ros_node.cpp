#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

#include <mrs_lib/subscriber_handler.h>

#include "behaviortree_cpp/bt_factory.h"

#include "utils/params.h"
#include "bt_nodes.cpp"

using namespace std::chrono_literals;

namespace example_simple_ros_node
{
    class SimpleBTNode : public rclcpp::Node
    {
    public:
        SimpleBTNode(const rclcpp::NodeOptions& options);
    private:
        rclcpp::Node::SharedPtr node_;
        bool loaded_successfully = true;

        /* is set to true when the nodelet is initialized, useful for rejecting callbacks that are called before the node is initialized */
        std::atomic<bool> is_initialized_ = false;

        rclcpp::TimerBase::SharedPtr timer_;
        void timer_callback();

        std::chrono::duration<double> timer_delay;
        std::string tree_file_name;
        int tick_repetition;
        int tick_counter = 0;

        std::shared_ptr<mrs_lib::SubscriberHandler<std_msgs::msg::String>> subscriber_string;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_string;

        BT::BehaviorTreeFactory factory;
        BT::Tree bt_tree;
        void registerNodes(BT::BehaviorTreeFactory& factory);

        rclcpp::TimerBase::SharedPtr timer_initializer_;
        void initialize();
    };

    SimpleBTNode::SimpleBTNode(const rclcpp::NodeOptions& options): Node("example_waypoint_flier_simple", options)
    {
        timer_initializer_ = create_wall_timer(std::chrono::duration<double>(1.0), std::bind(&SimpleBTNode::initialize,this));
    }

    void SimpleBTNode::initialize()
    {
        node_ = this->shared_from_this();

        /* Load parameters */
        loaded_successfully &= utils::load_param("bt_file_name", tree_file_name, std::string("bt_tree.xml"), *node_);
        loaded_successfully &= utils::load_param("tick_repetition", tick_repetition, 1, *node_);
        
        if (!loaded_successfully)
        {
            RCLCPP_INFO_ONCE(node_->get_logger(),"Failed to load non optional parameters");
        }
    
        /* Create subscriber handler */
        mrs_lib::SubscriberHandlerOptions shopts;
        shopts.node               = node_;
        shopts.node_name          = "bt_ros_node";
        shopts.no_message_timeout = rclcpp::Duration(5s);
        shopts.threadsafe         = true;
        shopts.autostart          = true;

        subscriber_string = std::make_shared<mrs_lib::SubscriberHandler<std_msgs::msg::String>>(shopts, "string_counter");

        /* Create publisher*/
        publisher_string = this->create_publisher<std_msgs::msg::String>("bt_output", 10);

        /* Register nodes and load tree from file (file name passed as parameter in config)*/
        registerNodes(factory);

        try {
            factory.registerBehaviorTreeFromFile(ament_index_cpp::get_package_share_directory("simple_sub_pub") + "/behavior_tree/" + tree_file_name);
        }
        catch (const std::exception& e) {
            RCLCPP_ERROR(this->get_logger(), "[BTNode]: Error while registering the tree from file %s/behavior_tree/tree.xml", ament_index_cpp::get_package_share_directory("ros_behaviortree_examples").c_str());
            rclcpp::shutdown();
            return;
        }

        bt_tree = factory.createTree("main_tree");
        
        /* Create timer for ticking the tree */
        timer_ = this->create_wall_timer(1s, std::bind(&SimpleBTNode::timer_callback,this));

        is_initialized_ = true;
        timer_initializer_->cancel();
    }

    void SimpleBTNode::registerNodes(BT::BehaviorTreeFactory& factory){
        factory.registerNodeType<BTNodes::StringTopicToBT>("StringTopicToBT", subscriber_string, this->get_logger());
        factory.registerNodeType<BTNodes::Logger>("Logger", this->get_logger());
        factory.registerNodeType<BTNodes::BTToStringTopic>("BTToStringTopic", publisher_string, this->get_logger());
    }

    void SimpleBTNode::timer_callback(){
        if (!is_initialized_) {
            return;
        }

        RCLCPP_INFO(this->get_logger(), "[BTNode]: Ticking the tree (%i/%i)", tick_counter, tick_repetition);
        if (bt_tree.tickOnce() == BT::NodeStatus::SUCCESS) {
            tick_counter++;
        }

        if(tick_counter > tick_repetition){
            RCLCPP_INFO(this->get_logger(), "[BTNode]: Finished ticking the tree, shutting down");
            rclcpp::shutdown();
        }
    }
}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(example_simple_ros_node::SimpleBTNode);