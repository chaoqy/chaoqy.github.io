---
layout: post
title: Modbus A/T Sensor
description: Developed and tested firmware for an ATtiny816-based Modbus RTU slave device that acquires accelerometer and temperature data.
skills:
- AVR
- Arduino
- Modbus
- RS485
- SPI
- UART
- C
main-image: /modbus_sensor.jpg
---

---
# Modbus A/T Sensor
Main firmware runs on an ATtiny816 and provides acceleration and temperature measurements over a Modbus RTU interface. The ATtiny816 acts as a Modbus slave device connected through an RS485 transceiver, while an external controller (Arduino) acts as the Modbus master.

Sensor data is acquired locally by the ATtiny816 using SPI communication with a KX132-1211 accelerometer and a thermistor interface. The firmware supports:
•	Reading temperature data (in NTU resistance)
•	Reading X-, Y-, and Z-axis acceleration data (in counts)
•	Reading acceleration acquisition timestamps (in ticks)
•	Changing the device Modbus ID
•	Changing the UART baud rate
•	Storing configuration settings in EEPROM so they persist across power cycles

Communication between the master and the ATtiny816 follows the Modbus RTU protocol. Requests are received over RS485, processed by the ATtiny816, and responded to with sensor data or configuration acknowledgements.
