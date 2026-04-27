Engineering materials
====
This are the material that were used for the development of this project:

1. x2 sensor holders
2. camera holder
3. L289N DC motor controller holder
4. arduino r4 holder
5. printed circuit board holder
6. arduino nicla holder
7. dc motor holder
8. servo motor holder
9. x4 Hex coupler connector
10. x5 wheels
11. x2 Axle carriers
12. x2 meal shaft
13. L289N DC motor controller
14. DC motor
15. MG996R Servo motor
16. Arduino UNO R4
17. x2 VL53L0 Laser sensor
18. Wires
19. Printed Circuit board
20. Pins
21. Arduino nicla
22. x4 resistors 10k
23. x2 mosfet 27000n
24. button



## Introduction

The robot moves using a DC motor controlled by an H-bridge motor driver. It uses three pins, IN1, IN2, and ENA, to set the direction and speed. The move() function allows the robot to go forward or backward, while the stop() function cuts the power to the motor to make it stop. This setup gives the robot basic but effective movement control, allowing it to travel through its environment smoothly.The robot is powered by a VEX 7.2V rechargeable battery. This battery is strong and reliable, designed for student robotics. It can power the motors and the electronics at the same time without losing voltage. The battery is rechargeable, safe, and long-lasting, which makes it perfect for classroom or project use. 


The speed of the motor is controlled using the analogWrite() function on the ENA pin. This uses a method called PWM (pulse width modulation), which lets the robot change speed without turning the motor completely on or off. The code also uses the absolute value of the speed and checks the direction so that negative numbers make the motor move in reverse without causing errors. This makes the robot's movement flexible and safe.

To complete the obstacle course, the system integrates the Arduino Nicla Sense ME. Communication between the Nicla and the main Arduino R4 is achieved through I2C, with the Nicla acting as a slave and the R4 as the master. Since the Nicla operates at 3.3V and the R4 at 5V, two level shifters are used—one for the SDA line and another for the SCL line—to safely handle voltage differences between the boards. This setup allows the Nicla to process sensor data or perform onboard computation while maintaining synchronized communication with the main controller during the robot’s operation.




