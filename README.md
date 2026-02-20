# Autonomous Color Classification & Sorting System

An embedded C++ system designed to identify specific color profiles using a TCS34725 RGB sensor and execute physical sorting via a multi-servo array. This project demonstrates real-time sensor data normalization, mathematical classification, and custom Pulse Width Modulation (PWM) logic.

## Technical Implementation

### 1. Mathematical Classification (Nearest Neighbor)
Rather than using simple thresholding, this system uses a **Euclidean Distance Algorithm** to classify colors. The system compares real-time normalized RGB ratios against a predefined reference array to find the "closest" match in a 3D color space.

$$d = \sqrt{(R_n - R_{ref})^2 + (G_n - G_{ref})^2 + (B_n - B_{ref})^2}$$

### 2. Low-Level Hardware Control
To ensure high precision and avoid the overhead of standard libraries, I implemented a **PWM signal generator** (`sendServoPulse`). This manually manages the 20ms duty cycle required for servo communication using microsecond-precision timing.
