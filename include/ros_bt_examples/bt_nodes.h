#pragma once

#include <mrs_lib/subscribe_handler.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <algorithm>
#include "behaviortree_cpp/bt_factory.h"
#include "ros_bt_examples/bt_nodes.hpp"

namespace ros_bt_examples
{

BT::NodeStatus log(BT::TreeNode& node);

class StringTopicToBB : public RosTopicToBB<std_msgs::String>
{
public:
  StringTopicToBB(const std::string&                                                  name,
                  const BT::NodeConfig&                                               config,
                  const std::shared_ptr<mrs_lib::SubscribeHandler<std_msgs::String>>& subscribe_handler);
};

class BBToStringTopic : public BBToRosTopic<std_msgs::String>
{
public:
  BBToStringTopic(const std::string&                     name,
                  const BT::NodeConfig&                  config,
                  const std::shared_ptr<ros::Publisher>& publisher);
};

class StandaloneSubPub : public BT::StatefulActionNode
{
public:
  StandaloneSubPub(const std::string&                                                  name,
                   const BT::NodeConfig&                                               config,
                   const std::shared_ptr<mrs_lib::SubscribeHandler<std_msgs::String>>& subscribe_handler,
                   const std::shared_ptr<ros::Publisher>&                              publisher);

  static BT::PortsList providedPorts();
  BT::NodeStatus       onStart() override;
  BT::NodeStatus       onRunning() override;
  void                 onHalted() override;

private:
  std::shared_ptr<std_msgs::String> m_string;

  std::shared_ptr<mrs_lib::SubscribeHandler<std_msgs::String>> m_sub;
  std::shared_ptr<ros::Publisher>                              m_pub;

  std::string capitalise(const std::string& input_str);
};
}  // namespace ros_bt_examples