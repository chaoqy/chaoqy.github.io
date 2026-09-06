---
layout: post
title: Modbus Sensor
description: Developed and tested firmware for a SAMG53-based Modbus RTU slave device that acquires accelerometer data.
skills:
- C
- ARM Cortex-M
- Modbus
- RS485
- SPI
- UART
main-image: /modbus_sensor.jpg
---

---
# Modbus Sensor
I designed and implemented embedded C firmware for a SAMG53-based sensor node that collects and analyzes accelerometer data using a KX132-1211 3-axis accelerometer over SPI. I wrote a custom SPI driver using direct register-level PDC (DMA) control to handle data transfers with minimal CPU involvement, allowing the system to efficiently acquire sensor data without constantly interrupting the processor. I also implemented an RTC-driven acquisition scheduler that collects 9,000 samples (3,000 per axis) every two minutes. 

To keep the system responsive, I separated the interrupt-driven scheduling from the acquisition process so that longer data collection operations would not block other peripherals. Finally, I exposed the processed sensor data through Modbus RTU over RS485, allowing the sensor node to communicate its results with external systems.
