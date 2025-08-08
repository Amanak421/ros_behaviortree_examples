#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <behaviortree_cpp/bt_factory.h>

#include "simple_report_node/msg/status.hpp"

using namespace BT;

namespace BTNodes 
{

    class RandomNode : public SyncActionNode 
    {
        public:
            RandomNode(const std::string& name, const NodeConfig& config) : SyncActionNode(name, config){}

            static PortsList providedPorts()
            {
                return {};
            }

            NodeStatus tick() override
            {
                if(rand() % 2 == 0){
                    return NodeStatus::SUCCESS;
                }else{
                    return NodeStatus::FAILURE;
                }
            }
    };

    class StatusReporter : public DecoratorNode
    {
        public:
            StatusReporter(const std::string& name, const NodeConfig& config, rclcpp::Publisher<simple_report_node::msg::Status>::SharedPtr handler, rclcpp::Logger logger) :
                DecoratorNode(name, config), pub_handler(handler), node_logger(logger){}

            static PortsList providedPorts()
            {
                return {};
            }

            BT::NodeStatus tick() override 
            {
                BT::NodeStatus child_status = child_node_->executeTick();
                auto msg = simple_report_node::msg::Status();

                switch (child_status)
                {
                case BT::NodeStatus::SUCCESS:
                    msg.node_status = msg.SUCCESS;
                    break;

                case BT::NodeStatus::FAILURE:
                    msg.node_status = msg.FAILURE;
                    break;

                case BT::NodeStatus::RUNNING:
                    msg.node_status = msg.RUNNING;
                    break;
                
                default:
                    msg.node_status = msg.UNKNOWN;
                    break;
                }

                pub_handler->publish(msg);

                return child_status;  // Pass it through
            }

        private:
            rclcpp::Publisher<simple_report_node::msg::Status>::SharedPtr pub_handler;
            rclcpp::Logger node_logger;
    };

}

