import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import serial

class CmdVelToArduino(Node):
    def __init__(self):
        super().__init__('cmd_vel_to_arduino')

        # Open serial port (adjust!)
        self.ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.listener_callback,
            10
        )

        self.get_logger().info("CmdVel → Arduino bridge started")

    def listener_callback(self, msg):
        linear = msg.linear.x
        angular = msg.angular.z

        # Convert to left/right wheel speeds
        wheel_base = 0.20  # meters (distance between wheels)

        left = linear - (angular * wheel_base / 2.0)
        right = linear + (angular * wheel_base / 2.0)

        # Scale to PWM range (-255 to 255)
        scale = 200
        left_pwm = int(left * scale)
        right_pwm = int(right * scale)

        # Clamp values
        left_pwm = max(-255, min(255, left_pwm))
        right_pwm = max(-255, min(255, right_pwm))

        # Send as CSV string
        command = f"{left_pwm},{right_pwm}\n"
        self.ser.write(command.encode())

        self.get_logger().info(f"Sent: {command.strip()}")


def main(args=None):
    rclpy.init(args=args)
    node = CmdVelToArduino()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()
