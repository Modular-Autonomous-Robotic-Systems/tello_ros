#include "tello_joy_node.hpp"

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "tello_msgs/srv/tello_action.hpp"

namespace tello_joy
{

  
  TelloJoyNode::TelloJoyNode(const rclcpp::NodeOptions &options) :
    rclcpp_lifecycle::LifecycleNode("tello_joy", options)
  {
	  rcl_interfaces::msg::ParameterDescriptor droneNameDesc;
	  droneNameDesc.description = "Path to output file for received video";
	  droneNameDesc.type = 4;
	  this->declare_parameter<std::string>("drone_name", "drone1", droneNameDesc);
  }
  
  TelloJoyNode::~TelloJoyNode()
  {}

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  TelloJoyNode::on_configure(const rclcpp_lifecycle::State & previous_state)
  {
    RCLCPP_INFO(get_logger(), "Configuring TelloJoy node...");
    
	mpDroneName = this->get_parameter("drone_name").as_string();

    using std::placeholders::_1;
    joy_sub_ = create_subscription<sensor_msgs::msg::Joy>(
      "joy", 1, 
      std::bind(&TelloJoyNode::joy_callback, this, _1));
      

	std::string cmdVelTopic = "/" + mpDroneName + "/cmd_vel";
    cmd_vel_pub_ = create_publisher<geometry_msgs::msg::Twist>(cmdVelTopic, 1);

    tello_client_ = create_client<tello_msgs::srv::TelloAction>("tello_action");

    (void) joy_sub_;

    RCLCPP_INFO(get_logger(), "Node configured");
    return CallbackReturn::SUCCESS;
  }

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  TelloJoyNode::on_activate(const rclcpp_lifecycle::State & previous_state)
  {
    RCLCPP_INFO(get_logger(), "Activating command controller in TelloJoy node...");
    
    // Activate the publisher

    cmd_vel_pub_->on_activate();
    
    RCLCPP_INFO(get_logger(), "Controller activated");
    return CallbackReturn::SUCCESS;
  }

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  TelloJoyNode::on_deactivate(const rclcpp_lifecycle::State & previous_state)
  {
    RCLCPP_INFO(get_logger(), "Deactivating command controller in TelloJoy node...");
    
    // Deactivate the publisher
    cmd_vel_pub_->on_deactivate();
    
    RCLCPP_INFO(get_logger(), "controller deactivated");
    return CallbackReturn::SUCCESS;
  }

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  TelloJoyNode::on_cleanup(const rclcpp_lifecycle::State & previous_state)
  {
    RCLCPP_INFO(get_logger(), "Cleaning up TelloJoy node...");
    
    // Reset all member variables
    joy_sub_.reset();
    cmd_vel_pub_.reset();
    tello_client_.reset();
    
    RCLCPP_INFO(get_logger(), "Node cleaned up");
    return CallbackReturn::SUCCESS;
  }

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  TelloJoyNode::on_shutdown(const rclcpp_lifecycle::State & previous_state)
  {
    RCLCPP_INFO(get_logger(), "Shutting down TelloJoy node...");
    
    // Reset all member variables
    joy_sub_.reset();
    cmd_vel_pub_.reset();
    tello_client_.reset();
    
    RCLCPP_INFO(get_logger(), "Node shut down");
    return CallbackReturn::SUCCESS;
  }
  
  void TelloJoyNode::joy_callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
  {
    if (joy_msg->buttons[joy_button_takeoff_]) {
      auto request = std::make_shared<tello_msgs::srv::TelloAction::Request>();
      request->cmd = "takeoff";
      tello_client_->async_send_request(request);
      RCLCPP_INFO(get_logger(), "request takeoff");
    } else if (joy_msg->buttons[joy_button_land_]) {
      auto request = std::make_shared<tello_msgs::srv::TelloAction::Request>();
      request->cmd = "land";
      tello_client_->async_send_request(request);
      RCLCPP_INFO(get_logger(), "request landing");
    } else {
      geometry_msgs::msg::Twist twist_msg;
      twist_msg.linear.x = joy_msg->axes[joy_axis_throttle_];
      twist_msg.linear.y = joy_msg->axes[joy_axis_strafe_];
      twist_msg.linear.z = joy_msg->axes[joy_axis_vertical_];
      twist_msg.angular.z = joy_msg->axes[joy_axis_yaw_];
      cmd_vel_pub_->publish(twist_msg);
      //RCLCPP_INFO(get_logger(), "sending rc command");
    }
  }

} // namespace tello_joy

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(tello_joy::TelloJoyNode)
