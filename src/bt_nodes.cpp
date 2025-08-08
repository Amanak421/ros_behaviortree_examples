#include <mrs_lib/subscriber_handler.h>
#include <std_msgs/msg/string.hpp>
#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

namespace BTNodes 
{

    class StringTopicToBT : public SyncActionNode 
    {
        public:
            StringTopicToBT(const std::string& name, const NodeConfig& config, std::shared_ptr<mrs_lib::SubscriberHandler<std_msgs::msg::String>> handler, rclcpp::Logger logger) : 
                SyncActionNode(name, config), sub_handler(handler), node_logger(logger){}

            static PortsList providedPorts()
            {
                return {OutputPort<std::string>("ros_topic")};
            }

            NodeStatus tick()
            {
                if(sub_handler->hasMsg()){
                    std::cout << "[BTree] Received message: " << (*sub_handler->getMsg()).data << std::endl;
                    return NodeStatus::SUCCESS;
                }else{
                    RCLCPP_WARN(node_logger, "[BHTree] Subscriber has no message!");
                    return NodeStatus::FAILURE;
                }
            }

        private:
            std::shared_ptr<mrs_lib::SubscriberHandler<std_msgs::msg::String>> sub_handler;
            rclcpp::Logger node_logger;
    };

    class Log : public SyncActionNode
    {
        public:
            Log(const std::string& name, const NodeConfig& config): SyncActionNode(name, config){}

            static PortsList providedPorts()
            {
                return {InputPort<std::string>("ros_topic")};
            }

            NodeStatus tick()
            {
                return NodeStatus::SUCCESS;
            }

    };

}

