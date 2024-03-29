#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui/highgui.hpp>
#include <ncurses.h>


class RobotController : public rclcpp::Node
{
public:
    RobotController() : Node("robot_controller")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
        subscriber_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10,
            std::bind(&RobotController::imageCallback, this, std::placeholders::_1));
        initKeyboard();
    }

    ~RobotController()
    {
        endwin();
    }

    void controlLoop()
    {
        int ch;
        nodelay(stdscr, TRUE);
        while (rclcpp::ok()) {
            if ((ch = getch()) != ERR) {
                geometry_msgs::msg::Twist msg;
                switch (ch) {
                    case 'w': 
                        msg.linear.x = 0.5;
                        break;
                    case 's': 
                        msg.linear.x = -0.5;
                        break;
                    case 'a':
                        msg.angular.z = 1.0;
                        break;
                    case 'd':
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

    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
        {
            try {
                cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;

                // Focando na parte inferior da imagem
                int focusHeight = frame.rows * 0.75; // Ajuste conforme necessário
                cv::Mat focusedFrame = frame(cv::Rect(0, focusHeight, frame.cols, frame.rows - focusHeight));

                cv::Mat edges;
                cv::cvtColor(focusedFrame, edges, cv::COLOR_BGR2GRAY);
                cv::GaussianBlur(edges, edges, cv::Size(3,3), 1.5, 1.5);
                cv::Canny(edges, edges, 0, 30, 3);

                // Procedimento padrão de divisão e contagem
                int width = edges.cols;
                int height = edges.rows;
                int segmentWidth = width / 3;
                cv::Rect leftRegion(0, 0, segmentWidth, height);
                cv::Rect centerRegion(segmentWidth, 0, segmentWidth, height);
                cv::Rect rightRegion(2 * segmentWidth, 0, segmentWidth, height);

                int leftCount = cv::countNonZero(edges(leftRegion));
                int centerCount = cv::countNonZero(edges(centerRegion));
                int rightCount = cv::countNonZero(edges(rightRegion));

                geometry_msgs::msg::Twist move_msg;
                if(centerCount > 10) { 
                    if(leftCount < rightCount) {
                        move_msg.angular.z = 1.0;
                    } else {
                        move_msg.angular.z = -1.0;
                    }
                } else {
                    move_msg.linear.x = 0.5;
                    move_msg.angular.z = 0.0; 
                }

                publisher_->publish(move_msg);

                cv::imshow("Camera Image", frame);
                cv::imshow("Focused Edges", edges);
                cv::waitKey(1);
            } catch (cv_bridge::Exception& e) {
                RCLCPP_ERROR(this->get_logger(), "Não foi possível converter a mensagem ROS para OpenCV: %s", e.what());
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
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscriber_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RobotController>();
    node->controlLoop();
    rclcpp::shutdown();
    return 0;
}
