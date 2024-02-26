#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class RobotController : public rclcpp::Node
{
public:
    RobotController() : Node("robot_controller")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
        timer_ = this->create_wall_timer(500ms, std::bind(&RobotController::publish_velocity, this));
    }

private:
    void publish_velocity()
    {
        geometry_msgs::msg::Twist msg;
        msg.linear.x = 0.5; // Move forward at 0.5 m/s
        msg.angular.z = 0.0; // No angular velocity
        publisher_->publish(msg);
    }
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RobotController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
