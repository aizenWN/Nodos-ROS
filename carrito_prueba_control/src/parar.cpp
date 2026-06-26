#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

class MotorController : public rclcpp::Node
{
public:
  MotorController() : Node("nodo_parar")
  {
    // Creamos el publicador apuntando al t�pico del controlador de velocidad
    publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "/velocity_controller/commands", 10);

    // Creamos un timer para que ejecute la funci�n 'enviar_comando' cada 100 milisegundos (10 Hz)
    timer_ = this->create_wall_timer(
      100ms, std::bind(&MotorController::enviar_comando, this));
  }

private:
  void enviar_comando()
  {
    auto mensaje = std_msgs::msg::Float64MultiArray();
    
    // Los mismos valores de velocidad para tus 4 articulaciones: [2.0, -2.0, 2.0, -2.0]
    mensaje.data = {0.0, 0.0, 0.0, 0.0};

    RCLCPP_INFO(this->get_logger(), "Enviando parar (freno)");
    publisher_->publish(mensaje);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MotorController>());
  rclcpp::shutdown();
  return 0;
}
