#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

enum EstadoMovimiento {
  ADELANTE,
  ATRAS,
  GIRO_DERECHA,
  GIRO_IZQUIERDA
};

class MotorController : public rclcpp::Node
{
public:
  MotorController() : Node("nodo_direcciones_prueba"), ciclos(0), estadoActual(ADELANTE)
  {
    // Creamos el publicador apuntando al topico del controlador de velocidad
    publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "/velocity_controller/commands", 10);

    // Creamos un timer para que ejecute la funcion 'enviar_comando' cada 100 milisegundos (10 Hz)
    timer_ = this->create_wall_timer(
      100ms, std::bind(&MotorController::enviar_comando, this));
  }

  void frenar_motores()
  {
    RCLCPP_INFO(this->get_logger(), "Deteniendo motores por seguridad...");
    auto mensajeFreno = std_msgs::msg::Float64MultiArray();
    mensajeFreno.data = {0.0, 0.0, 0.0, 0.0};

    for(int i = 0; i < 10; i++) {
      publisher_->publish(mensajeFreno);
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

private:
  void enviar_comando()
  {
    auto mensaje = std_msgs::msg::Float64MultiArray();
    
    switch(estadoActual) {
      case ADELANTE:
        mensaje.data = {2.0, 2.0, 2.0, 2.0};
        RCLCPP_INFO(this->get_logger(), "Enviando DIRECCIONES: ADELANTE");
        break;
      case ATRAS:
        mensaje.data = {-2.0, -2.0, -2.0, -2.0};
        RCLCPP_INFO(this->get_logger(), "Enviando DIRECCIONES: ATRAS");
        break;
      case GIRO_DERECHA:
        mensaje.data = {2.0, 2.0, -2.0, -2.0};
        RCLCPP_INFO(this->get_logger(), "Enviando DIRECCIONES: GIRO DERECHA");
        break;
      case GIRO_IZQUIERDA:
        mensaje.data = {-2.0, -2.0, 2.0, 2.0};
        RCLCPP_INFO(this->get_logger(), "Enviando DIRECCIONES: GIRO IZQUIERDA");
        break;
    }

    publisher_->publish(mensaje);
    ciclos++;

    if(ciclos >= 30){
      ciclos = 0;
      
      if(estadoActual == ADELANTE){
        estadoActual = ATRAS;
      } else if(estadoActual == ATRAS){
        estadoActual = GIRO_DERECHA;
      } else if(estadoActual == GIRO_DERECHA){
        estadoActual = GIRO_IZQUIERDA;
      } else if(estadoActual == GIRO_IZQUIERDA){
        estadoActual = ADELANTE;
      }
    }
  }
  int ciclos;
  EstadoMovimiento estadoActual;


  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto nodo = std::make_shared<MotorController>();

  std::thread thread_spin([nodo]() {
    rclcpp::spin(nodo);
  });

  while (rclcpp::ok()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  nodo->frenar_motores();

  rclcpp::shutdown();
  if (thread_spin.joinable()) {
    thread_spin.join();
  }

  return 0;
}
