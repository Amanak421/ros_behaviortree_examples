#include "ros_bt_examples/bt_nodes.h"

namespace ros_bt_examples
{

BT::NodeStatus log(BT::TreeNode& self)
{
  auto in_string = self.getInput<std::shared_ptr<std_msgs::String>>("in_string");
  if (in_string.has_value()) {
    ROS_INFO_STREAM("[Tree Node " << self.name().c_str() << "]: Logging string: " << in_string.value()->data);
    return BT::NodeStatus::SUCCESS;
  }
  else {
    ROS_WARN_STREAM("[Tree Node " << self.name().c_str() << "]: Input port: 'in_string' is empty");
    return BT::NodeStatus::FAILURE;
  }
}

StringTopicToBB::StringTopicToBB(const std::string&                                                  name,
                                 const BT::NodeConfig&                                               config,
                                 const std::shared_ptr<mrs_lib::SubscribeHandler<std_msgs::String>>& subscribe_handler)
  : RosTopicToBB(name,
                 config,
                 subscribe_handler)
{
}

BBToStringTopic::BBToStringTopic(const std::string&                     name,
                                 const BT::NodeConfig&                  config,
                                 const std::shared_ptr<ros::Publisher>& publisher)
  : BBToRosTopic(name,
                 config,
                 publisher)
{
}

StandaloneSubPub::StandaloneSubPub(
    const std::string&                                                  name,
    const BT::NodeConfig&                                               config,
    const std::shared_ptr<mrs_lib::SubscribeHandler<std_msgs::String>>& subscribe_handler,
    const std::shared_ptr<ros::Publisher>&                              publisher)
  : StatefulActionNode(name,
                       config)
{
  m_pub = publisher;
  m_sub = subscribe_handler;
}

BT::PortsList StandaloneSubPub::providedPorts()
{
  return { BT::InputPort<std::string>("in_msg") };
}

BT::NodeStatus StandaloneSubPub::onStart()
{
  auto in_msg = getInput<std::string>("in_msg");
  if (in_msg.has_value()) {
    return BT::NodeStatus::RUNNING;
  }
  else {
    ROS_WARN_STREAM("[Tree Node " << name().c_str() << "]: Input ports are not set. Check the input ports.");
    return BT::NodeStatus::FAILURE;
  }
}

BT::NodeStatus StandaloneSubPub::onRunning()
{
  auto in_msg = getInput<std::string>("in_msg");

  if (!in_msg.has_value()) {
    ROS_ERROR_STREAM("[Tree Node " << name().c_str() << "]: Input port: 'in_msg' is empty");
    return BT::NodeStatus::FAILURE;
  }

  if (!m_sub->newMsg()) {
    ROS_WARN_STREAM("[Tree Node " << name().c_str() << "]: Subscriber has no msg");
    return BT::NodeStatus::RUNNING;
  }

  std_msgs::String msg_out;
  msg_out.data = m_sub->getMsg()->data + capitalise(in_msg.value());

  m_pub->publish(msg_out);
  ROS_INFO_STREAM("[Tree Node " << name().c_str() << "]: Published topic " << m_pub->getTopic().c_str());
  return BT::NodeStatus::SUCCESS;
}

void StandaloneSubPub::onHalted()
{
  return;
}

std::string StandaloneSubPub::capitalise(const std::string& input_str)
{
  std::string new_str = input_str;
  std::transform(new_str.begin(), new_str.end(), new_str.begin(), ::toupper);
  return new_str;
}
}  // namespace ros_bt_examples