#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

#include "behaviortree_cpp/bt_factory.h"

#include "utils/params.h"
#include "bt_nodes.cpp"

using namespace std::chrono_literals;

namespace example_simple_client_node
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

        BT::BehaviorTreeFactory factory;
        BT::Tree bt_tree;
        void registerNodes(BT::BehaviorTreeFactory& factory);

        rclcpp::Node::SharedPtr client_node;
        RosNodeParams client_node_params; 
        

        rclcpp::TimerBase::SharedPtr timer_initializer_;
        void initialize();
    };

    SimpleBTNode::SimpleBTNode(const rclcpp::NodeOptions& options): Node("example_report_node", options)
    {
        timer_initializer_ = create_wall_timer(std::chrono::duration<double>(1.0), std::bind(&SimpleBTNode::initialize,this));
    }

    void SimpleBTNode::initialize()
    {
        node_ = this->shared_from_this();

        /* Load parameters */
        loaded_successfully &= utils::load_param("bt_file_name", tree_file_name, std::string("bt_tree.xml"), *node_);
        if (!loaded_successfully)
        {
            RCLCPP_INFO_ONCE(node_->get_logger(),"Failed to load non optional parameters");
        }

        /* Client node for BT */
        client_node = std::make_shared<rclcpp::Node>("fibonacci_action_client");
        client_node_params.nh = client_node;
        client_node_params.default_port_value = "fibonacci";

        /* Register nodes and load tree from file (file name passed as parameter in config)*/
        registerNodes(factory);

        try {
            factory.registerBehaviorTreeFromFile(ament_index_cpp::get_package_share_directory("simple_server_client") + "/behavior_tree/" + tree_file_name);
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
        factory.registerNodeType<BTNodes::InputField>("InputField", node_->get_logger(), 5);
        factory.registerNodeType<BTNodes::FibonacciAction>("Fibonacci", client_node_params);
    }

    void SimpleBTNode::timer_callback(){
        if (!is_initialized_) {
            return;
        }

        RCLCPP_INFO(this->get_logger(), "[BTNode]: Ticking the tree (%i/%i)", tick_counter, tick_repetition);
        bt_tree.tickWhileRunning();

        RCLCPP_INFO(this->get_logger(), "[BTNode]: Finished ticking the tree, shutting down");
        rclcpp::shutdown();
    }
}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(example_simple_client_node::SimpleBTNode);