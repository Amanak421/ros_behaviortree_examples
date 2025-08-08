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

            NodeStatus tick() override

            {
                if(sub_handler->hasMsg()){
                    setOutput("ros_topic", (*sub_handler->getMsg()).data);
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

    class Logger : public SyncActionNode
    {
        public:
            Logger(const std::string& name, const NodeConfig& config, rclcpp::Logger logger): SyncActionNode(name, config), node_logger(logger){}

            static PortsList providedPorts()
            {
                return {InputPort<std::string>("ros_topic")};
            }

            NodeStatus tick() override
            {
                auto msg = getInput<std::string>("ros_topic");
                if(!msg){
                    RCLCPP_ERROR(node_logger, "Missing input to node (ros_topic)!");
                    return NodeStatus::FAILURE;
                }
                RCLCPP_INFO(node_logger, "Received message: %s", msg.value().c_str());
                return NodeStatus::SUCCESS;
            }

        private:
            rclcpp::Logger node_logger;

    };

    class BTToStringTopic : public SyncActionNode
    {
        public:
            BTToStringTopic(const std::string& name, const NodeConfig& config, rclcpp::Publisher<std_msgs::msg::String>::SharedPtr handler, rclcpp::Logger logger) : 
                SyncActionNode(name, config), pub_handler(handler), node_logger(logger){}

            static PortsList providedPorts()
            {
                return {InputPort<std::string>("ros_topic")};
            }

            NodeStatus tick() override
            {
                auto msg = getInput<std::string>("ros_topic");
                if(!msg){
                    RCLCPP_ERROR(node_logger, "Missing input to node (ros_topic)!");
                    return NodeStatus::FAILURE;
                }

                auto message = std_msgs::msg::String();
                message.data = msg.value();
                pub_handler->publish(message);
                return NodeStatus::SUCCESS;
            }

        
        private:
            rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_handler;
            rclcpp::Logger node_logger;
    };

}

