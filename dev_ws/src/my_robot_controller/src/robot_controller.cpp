#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <ncurses.h>

class RobotController : public rclcpp::Node
{
public:
    RobotController() : Node("robot_controller")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
        initKeyboard();
    }

    ~RobotController()
    {
        endwin(); // Finaliza o modo ncurses
    }

    void controlLoop()
    {
        int ch;
        nodelay(stdscr, TRUE);
        while (rclcpp::ok()) {
            if ((ch = getch()) != ERR) {
                geometry_msgs::msg::Twist msg;
                switch (ch) {
                    case 'w': // frente
                        msg.linear.x = 0.5;
                        break;
                    case 's': // trás
                        msg.linear.x = -0.5;
                        break;
                    case 'a': // esquerda
                        msg.angular.z = 1.0;
                        break;
                    case 'd': // direita
                        msg.angular.z = -1.0;
                        break;
                    default:
                        msg.linear.x = 0.0;
                        msg.angular.z = 0.0;
                        break;
                }
                publisher_->publish(msg);
            }
            rclcpp::spin_some(shared_from_this());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

private:
    void initKeyboard()
    {
        initscr();
        cbreak(); 
        noecho();
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RobotController>();
    node->controlLoop();
    rclcpp::shutdown();
    return 0;
}
