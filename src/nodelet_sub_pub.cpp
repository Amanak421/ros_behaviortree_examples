#include <behaviortree_cpp/blackboard.h>
#include <behaviortree_cpp/bt_factory.h>
#include <mrs_lib/param_loader.h>
#include <mrs_lib/subscribe_handler.h>
#include <mrs_lib/timer.h>
#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>
#include <ros/package.h>
#include <ros/ros.h>
#include <ros_bt_examples/bt_nodes.h>
#include <std_msgs/String.h>
#include <algorithm>
#include <fstream>
#include "behaviortree_cpp/xml_parsing.h"

namespace ros_bt_examples
{

class SubPubExample : public nodelet::Nodelet
{
public:
  virtual void onInit();

private:
  bool        m_is_initialized = false;
  std::string m_node_name      = "SubPubExample";

  BT::Blackboard::Ptr     m_global_blackboard;
  BT::BehaviorTreeFactory m_factory;
  BT::Tree                m_behavior_tree;

  std::string m_p_behavior_file;

  // | ----------------------- subscribers ---------------------- |

  std::shared_ptr<mrs_lib::SubscribeHandler<std_msgs::String>> m_sub_string;

  // | ----------------------- publishers ----------------------- |

  std::shared_ptr<ros::Publisher> m_pub_string;

  std::shared_ptr<mrs_lib::ROSTimer> m_ros_timer;

  void registerBTNodes(BT::BehaviorTreeFactory& factory);
  void timerTickTree([[maybe_unused]] const ros::TimerEvent& evt);
};

void SubPubExample::onInit()
{
  m_is_initialized   = false;
  ros::NodeHandle nh = nodelet::Nodelet::getMTPrivateNodeHandle();
  ros::Time::waitForValid();

  mrs_lib::ParamLoader param_loader{ nh, m_node_name };
  double               rate = 0;
  param_loader.loadParam("timer/rate", rate);
  param_loader.loadParam("bt/behavior_file", m_p_behavior_file);

  if (!param_loader.loadedSuccessfully()) {
    ROS_ERROR_STREAM("[" << m_node_name.c_str() << "]: parameter loading failure! Shutting down the node");
    ros::shutdown();
    return;
  }

  mrs_lib::SubscribeHandlerOptions shopts;
  shopts.nh                 = nh;
  shopts.node_name          = m_node_name;
  shopts.no_message_timeout = ros::Duration(5.0);
  shopts.threadsafe         = true;
  shopts.autostart          = true;
  shopts.queue_size         = 10;
  shopts.transport_hints    = ros::TransportHints().tcpNoDelay();

  m_sub_string = std::make_shared<mrs_lib::SubscribeHandler<std_msgs::String>>(shopts, "sub_str");

  m_pub_string = std::make_shared<ros::Publisher>(nh.advertise<std_msgs::String>("pub_log", 1));

  m_ros_timer = std::make_unique<mrs_lib::ROSTimer>(nh, ros::Rate{ rate }, &SubPubExample::timerTickTree, this);

  registerBTNodes(m_factory);
  try {
    m_factory.registerBehaviorTreeFromFile(ros::package::getPath("ros_bt_examples") + "/behavior_tree/" +
                                           m_p_behavior_file);
  }
  catch (const std::exception& e) {
    ROS_ERROR_STREAM("[" << m_node_name.c_str() << "]: Error while registerging the tree from file");
    ros::shutdown();
    return;
  }

  m_behavior_tree = m_factory.createTree("main_tree", BT::Blackboard::create());

  m_is_initialized = true;
  ROS_INFO_STREAM("[" << m_node_name.c_str() << "]: initialized");
}

void SubPubExample::registerBTNodes(BT::BehaviorTreeFactory& factory)
{
  factory.registerNodeType<StringTopicToBB>("StringTopicToBB", m_sub_string);
  factory.registerNodeType<BBToStringTopic>("BBToStringTopic", m_pub_string);
  factory.registerSimpleAction("Log", log, { BT::InputPort<std::shared_ptr<std_msgs::String>>("in_string") });
  factory.registerNodeType<StandaloneSubPub>("StandaloneSubPub", m_sub_string, m_pub_string);

  auto          xml_models = BT::writeTreeNodesModelXML(factory);
  std::ofstream out{ ros::package::getPath("ros_bt_examples") + "/behavior_tree/" + "models.xml" };
  out << xml_models;
  out.close();
}

void SubPubExample::timerTickTree([[maybe_unused]] const ros::TimerEvent& evt)
{
  if (!m_is_initialized) {
    return;
  }

  ROS_INFO_STREAM("[" << m_node_name.c_str() << "]: Ticking the tree");

  if (m_behavior_tree.tickOnce() == BT::NodeStatus::SUCCESS) {
    ROS_INFO_STREAM("[" << m_node_name.c_str() << "]: Finished the tree, shutting down");
    ros::shutdown();
  }
}
}  // namespace ros_bt_examples

PLUGINLIB_EXPORT_CLASS(ros_bt_examples::SubPubExample,
                       nodelet::Nodelet)