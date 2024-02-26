#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <memory>

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("robot_talker");
  auto publisher = node->create_publisher<std_msgs::msg::String>("chatter", rclcpp::QoS(10));
  rclcpp::WallRate loop_rate(2);

  while (rclcpp::ok()) {
    auto message = std::make_shared<std_msgs::msg::String>();
    message->data = "Hello, ROS 2 world!";
    RCLCPP_INFO(node->get_logger(), "Publishing: '%s'", message->data.c_str());
    publisher->publish(*message);
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  rclcpp::shutdown();
  return 0;
}
