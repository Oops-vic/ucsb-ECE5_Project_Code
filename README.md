# ucsb-ECE5_Project_Code
Arduino codes for ECE5 project

This is the repo for the final project of ECE5, winter 2020. The project aims to remotely control the traditional toggle switches by adding an attachment to each switch. We use Blynk to send the on/off commands to the Arduino BLE module via bluetooth. After receiving the signal, the bluetooth board will simulate PWM signals through a shift register to control the servo motors, which will drive the mechanical design of the attachment to follow the on/off commands.
