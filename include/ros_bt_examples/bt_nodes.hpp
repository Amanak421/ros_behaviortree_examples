#pragma once

#include <mrs_lib/subscribe_handler.h>
#include <ros/ros.h>
#include "behaviortree_cpp/bt_factory.h"

namespace ros_bt_examples
{
template <class TopicT>
class RosTopicToBB : public BT::SyncActionNode
{
public:
  RosTopicToBB(const std::string&                                        name,
               const BT::NodeConfig&                                     config,
               const std::shared_ptr<mrs_lib::SubscribeHandler<TopicT>>& subscribe_handler)
    : BT::SyncActionNode(name,
                         config)
    , m_subscribe_handler(subscribe_handler)
  {
  }

  static BT::PortsList providedPorts()
  {
    return { BT::OutputPort<std::shared_ptr<TopicT>>("out_key") };
  }

  BT::NodeStatus tick()
  {
    if (m_subscribe_handler->newMsg()) {
      setOutput("out_key", std::make_shared<TopicT>(*m_subscribe_handler->getMsg()));
      return BT::NodeStatus::SUCCESS;
    }
    else {
      ROS_WARN_STREAM("[Tree Node " << name().c_str() << "]: Subscriber has no msg");
      return BT::NodeStatus::FAILURE;
    }
  }

private:
  std::shared_ptr<mrs_lib::SubscribeHandler<TopicT>> m_subscribe_handler;
};

template <class TopicT>
class BBToRosTopic : public BT::SyncActionNode
{
public:
  BBToRosTopic(const std::string&                     name,
               const BT::NodeConfig&                  config,
               const std::shared_ptr<ros::Publisher>& publisher)
    : BT::SyncActionNode(name,
                         config)
    , m_publisher(publisher)
  {
  }

  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<std::shared_ptr<TopicT>>("in_key") };
  }

  BT::NodeStatus tick()
  {
    auto in_key = getInput<std::shared_ptr<TopicT>>("in_key");
    if (in_key.has_value()) {
      m_publisher->publish(*in_key.value());
      ROS_INFO_STREAM("[Tree Node " << name().c_str() << "]: Published topic " << m_publisher->getTopic().c_str());
      return BT::NodeStatus::SUCCESS;
    }
    else {
      ROS_WARN_STREAM("[Tree Node " << name().c_str() << "]: Input port: 'in_key' is empty");
      return BT::NodeStatus::FAILURE;
    }
  }

private:
  std::shared_ptr<ros::Publisher> m_publisher;
};
}  // namespace ros_bt_examples