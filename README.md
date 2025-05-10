The Aim Assist System is an integrated hardware-software solution that automatically adjusts the launch angle of a mechanical projectile launcher. The system operates in two modes:
Manual Mode: Real-time orientation data from an MPU6050 motion sensor
Camera Mode: Target position tracking using computer vision
This adaptive system dynamically computes optimal firing angles to enhance projectile accuracy, demonstrating practical applications of sensor fusion and computer vision in targeting systems
Design and implement a dual-mode aim assistance mechanism
Enable seamless switching between manual and automatic targeting
Provide real-time distance measurements to targets
Display operational parameters via integrated LCD
Develop a foundation for future enhancements in autonomous targeting systems
Controlled using the MPU6050 sensor
Reads pitch angle and converts it to servo rotation (0° to 180°)
Continuously displays angle and measured distance to the target using HC-SR04
Updates LCD with real-time sensor data
Python script processes video feed from laptop camera
Tracks a yellow target through HSV color filtering
Calculates vertical position of target and maps to a servo angle
Transmits angle data via Serial to Arduino
Arduino actuates the servo while displaying angle and distance information
MPU Mode: ±3° accuracy within normal operating range
Camera Mode: ±5° accuracy under optimal lighting conditions
Distance Measurement: ±2cm accuracy up to 4 meters
Mode Switching: <100ms
MPU Response: <250ms from movement to servo actuation
Camera Response: 100-300ms depending on lighting conditions
Camera mode requires consistent lighting for reliable target detection
Yellow object detection sensitive to ambient light and color interference
Ultrasonic sensor accuracy decreases with distance and target surface angle
