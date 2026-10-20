---
layout: post
title: Autonomous Rover
description: Developed an autonomous rover using ROS 2 and Raspberry Pi, implementing closed-loop PID control for navigation.
skills: 
- ROS 2
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

To tie everything together, I designed a system where each component handles a specific part of the robot’s behavior. Starting at the top, the LiDAR streams data through ROS 2 to the Raspberry Pi, where I process it using SLAM and Nav2 to build a map and plan paths in real time. From there, the Pi generates velocity commands, which I send down to the Arduino.

I use the Arduino as a low-level controller. It takes those commands and translates them into actual motor movement through the driver, while also reading feedback from wheel encoders and IR sensors. The encoders let me track wheel speed, and the IR sensors help detect nearby obstacles. I then send all of that feedback back to the Pi, where it updates odometry and continuously refines navigation decisions.

## Arduino Firmware
Firmware that turns velocity commands from a ROS 2 host into regulated wheel speeds. The microcontroller runs a 100 Hz real-time control loop. It measures each wheel’s speed from encoders, corrects it with a per-wheel PID controller, and streams encoder counts back to the host for odometry. Splitting the work this way keeps timing-critical motor control on the microcontroller and leaves planning and navigation to ROS 2.

### Technical Highlights
- **Command interface**: the host sends line-delimited commands (`V|v_left|v_right\n`) at 115200 baud, with target wheel speeds in m/s. A small parser with a bounded 64-byte buffer handles partial reads and oversized lines.
- **Encoder sensing**: each wheel has a quadrature encoder (3000 ticks per revolution). Interrupt service routines count rising edges on channel A and read channel B for direction. The main loop takes a snapshot of the counts inside a short interrupt-disabled critical section, so reads are race-free.
- **Velocity estimation**: per-interval tick deltas become angular velocity, then linear velocity using the 6.25 cm wheel radius. Cumulative tick totals are kept separately for odometry.
- **PID control**: each wheel has its own controller (Kp = 180, Ki = 80, Kd = 20) with proportional, integral, and derivative terms computed from the measured loop time. The output is clamped to ±250 PWM and drives an H-bridge, with direction pins for forward and reverse.
- **Scheduling**: a non-blocking loop on `millis()` runs sensing, control, and telemetry every 10 ms, while serial parsing and the safety check run on every pass.
- **Safety watchdog**: if no command arrives for 500 ms, the setpoints drop to zero and the integral terms reset, so the rover stops if the host crashes or the link drops.
- **IMU**: an LSM6DS3 accelerometer and gyroscope are initialized and sampled each cycle, giving the firmware inertial data for future use.

[Code link](https://github.com/chaoqy/chaoqy.github.io/blob/main/_projects/autonomous-rover/code)
