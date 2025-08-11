#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include <behaviortree_ros2/bt_action_node.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include "simple_server_client/action/fibonacci.hpp"

using Fibonacci = simple_server_client::action::Fibonacci;
using GoalHandleFibonacci = rclcpp_action::ServerGoalHandle<Fibonacci>;

using namespace BT;

namespace BTNodes{

    /* Example from https://www.behaviortree.dev/docs/ros2_integration */
    class FibonacciAction: public RosActionNode<Fibonacci>
    {
        public:
        FibonacciAction(const std::string& name,
                        const NodeConfig& conf,
                        const RosNodeParams& params)
            : RosActionNode<Fibonacci>(name, conf, params)
        {}

        // The specific ports of this Derived class
        // should be merged with the ports of the base class,
        // using RosActionNode::providedBasicPorts()
        static PortsList providedPorts()
        {
            return providedBasicPorts({InputPort<unsigned>("order")});
        }

        // This is called when the TreeNode is ticked and it should
        // send the request to the action server
        bool setGoal(RosActionNode::Goal& goal) override 
        {
            // get "order" from the Input port
            getInput("order", goal.order);
            // return true, if we were able to set the goal correctly.
            return true;
        }
        
        // Callback executed when the reply is received.
        // Based on the reply you may decide to return SUCCESS or FAILURE.
        NodeStatus onResultReceived(const WrappedResult& wr) override
        {
            std::stringstream ss;
            ss << "Result received: ";
            for (auto number : wr.result->sequence) {
            ss << number << " ";
            }
            if (auto node = node_.lock()) {  // node_ is std::weak_ptr<rclcpp::Node>
                RCLCPP_INFO(node->get_logger(), "%s", ss.str().c_str());
            } else {
                // handle expired pointer case if needed
            }
            return NodeStatus::SUCCESS;
        }

        // Callback invoked when there was an error at the level
        // of the communication between client and server.
        // This will set the status of the TreeNode to either SUCCESS or FAILURE,
        // based on the return value.
        // If not overridden, it will return FAILURE by default.
        virtual NodeStatus onFailure(ActionNodeErrorCode error) override
        {
            if (auto node = node_.lock()) {  // node_ is std::weak_ptr<rclcpp::Node>
                RCLCPP_ERROR(node->get_logger(), "Error: %d", error);
            } else {
                // handle expired pointer case if needed
            }
            return NodeStatus::FAILURE;
        }

        // we also support a callback for the feedback, as in
        // the original tutorial.
        // Usually, this callback should return RUNNING, but you
        // might decide, based on the value of the feedback, to abort
        // the action, and consider the TreeNode completed.
        // In that case, return SUCCESS or FAILURE.
        // The Cancel request will be send automatically to the server.
        NodeStatus onFeedback(const std::shared_ptr<const Feedback> feedback)
        {
            std::stringstream ss;
            ss << "Next number in sequence received: ";
            for (auto number : feedback->partial_sequence) {
            ss << number << " ";
            }
            if (auto node = node_.lock()) {  // node_ is std::weak_ptr<rclcpp::Node>
                RCLCPP_INFO(node->get_logger(), ss.str().c_str());
            } else {
                // handle expired pointer case if needed
            }
            return NodeStatus::RUNNING;
        }
    };

    class InputField : public SyncActionNode
    {
        public:
            InputField(const std::string& name, const NodeConfig& config, rclcpp::Logger logger, int fib_order) : SyncActionNode(name, config), node_logger(logger), order(fib_order){}

            static PortsList providedPorts()
            {
                return {OutputPort<unsigned>("order")};
            }

            NodeStatus tick() override
            {
                RCLCPP_INFO(node_logger, "Set order to: %i", order);
                setOutput("order", (unsigned int)order);
                return NodeStatus::SUCCESS;
            }
        private:
            rclcpp::Logger node_logger;
            int order;
    };

}