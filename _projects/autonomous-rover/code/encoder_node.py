#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

import serial
import math
import time

from nav_msgs.msg import Odometry
from sensor_msgs.msg import JointState
from geometry_msgs.msg import TransformStamped

from tf2_ros import TransformBroadcaster


class EncoderNode(Node):

    def __init__(self):
        super().__init__('encoder_node')

        self.serial = serial.Serial('/dev/ttyACM0', 115200)

        self.wheel_radius = 0.0625      # meters
        self.wheel_base = 0.30          # distance between wheels (m)
        self.ticks_per_rev = 3000

        self.dist_per_tick = (2 * math.pi * self.wheel_radius) / self.ticks_per_rev

        # ----- STATE -----
        self.prev_left = None
        self.prev_right = None

        self.x = 0.0
        self.y = 0.0
        self.theta = 0.0

        # ----- ROS INTERFACES -----
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
        self.joint_pub = self.create_publisher(JointState, '/joint_states', 10)

        self.tf_broadcaster = TransformBroadcaster(self)

        self.timer = self.create_timer(0.02, self.update)  # 50 Hz

        self.last_time = self.get_clock().now()

    def update(self):

        line = self.serial.readline().decode().strip()

        try:
            left_ticks, right_ticks = map(int, line.split(','))
        except:
            return

        if self.prev_left is None:
            self.prev_left = left_ticks
            self.prev_right = right_ticks
            return

        # ----- TICK DIFFERENCE -----
        delta_left = left_ticks - self.prev_left
        delta_right = right_ticks - self.prev_right

        self.prev_left = left_ticks
        self.prev_right = right_ticks

        # ----- DISTANCE -----
        d_left = delta_left * self.dist_per_tick
        d_right = delta_right * self.dist_per_tick

        d_center = (d_left + d_right) / 2
        d_theta = (d_right - d_left) / self.wheel_base

        # ----- UPDATE POSE -----
        self.x += d_center * math.cos(self.theta)
        self.y += d_center * math.sin(self.theta)
        self.theta += d_theta

        current_time = self.get_clock().now()
        dt = (current_time - self.last_time).nanoseconds / 1e9
        self.last_time = current_time

        # ----- ODOMETRY MESSAGE -----
        odom = Odometry()

        odom.header.stamp = current_time.to_msg()
        odom.header.frame_id = "odom"

        odom.child_frame_id = "base_link"

        odom.pose.pose.position.x = self.x
        odom.pose.pose.position.y = self.y

        odom.pose.pose.orientation.z = math.sin(self.theta / 2)
        odom.pose.pose.orientation.w = math.cos(self.theta / 2)

        odom.twist.twist.linear.x = d_center / dt
        odom.twist.twist.angular.z = d_theta / dt

        self.odom_pub.publish(odom)

        # ----- TF BROADCAST -----
        tf = TransformStamped()

        tf.header.stamp = current_time.to_msg()
        tf.header.frame_id = "odom"
        tf.child_frame_id = "base_link"

        tf.transform.translation.x = self.x
        tf.transform.translation.y = self.y

        tf.transform.rotation.z = math.sin(self.theta / 2)
        tf.transform.rotation.w = math.cos(self.theta / 2)

        self.tf_broadcaster.sendTransform(tf)

        # ----- JOINT STATES -----
        js = JointState()

        js.header.stamp = current_time.to_msg()

        js.name = ['left_wheel_joint', 'right_wheel_joint']
        js.position = [
            left_ticks * 2 * math.pi / self.ticks_per_rev,
            right_ticks * 2 * math.pi / self.ticks_per_rev
        ]

        self.joint_pub.publish(js)


def main():
    rclpy.init()

    node = EncoderNode()

    rclpy.spin(node)

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
