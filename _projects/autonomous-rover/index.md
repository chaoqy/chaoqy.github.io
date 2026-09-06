---
layout: post
title: Autonomous Rover
description: Developed an autonomous rover using ROS2 and Raspberry Pi, implementing closed-loop PID control for navigation.
skills: 
- ROS2
- Raspberry Pi
- Arduino
- Python
main-image: /rover(1).jpeg 
---

---
# Autonomous Rover
The rover works by having the Raspberry Pi acting as the brain of the robot, taking in feedback from the LiDAR, IMU measurements, IR sensors, and wheel encoders to tell the Arduino where to move to. 

## Robot Architecture Flowchart
{% include image-gallery.html images="architecture.png" %}  

To tie everything together, I designed a system where each component handles a specific part of the robot’s behavior. Starting at the top, the LiDAR streams data through ROS2 to the Raspberry Pi, where I process it using SLAM and Nav2 to build a map and plan paths in real time. From there, the Pi generates velocity commands, which I send down to the Arduino.

I use the Arduino as a low-level controller. It takes those commands and translates them into actual motor movement through the driver, while also reading feedback from wheel encoders and IR sensors. The encoders let me track wheel speed, and the IR sensors help detect nearby obstacles. I then send all of that feedback back to the Pi, where it updates odometry and continuously refines navigation decisions.

[Code link](https://github.com/chaoqy/chaoqy.github.io/blob/main/_projects/autonomous-rover/code)
