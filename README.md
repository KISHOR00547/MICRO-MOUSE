# MICRO-MOUSE
 <img width="720" height="1600" alt="image" src="https://github.com/user-attachments/assets/3ab6d26c-b327-48ad-b0eb-e8df9df40255" />

# Micromouse Maze Solver Robot

## Overview
This project is an autonomous **Micromouse maze-solving robot** designed to navigate a 10 cm maze using wall detection and intelligent path decision-making. The robot measures **8 × 8 cm**, making it compact enough for competition constraints while maintaining stable navigation performance.

The robot uses **Time-of-Flight (ToF) laser sensors**, encoder-based motion control, and an STM32 microcontroller to detect walls, make directional decisions, and solve the maze efficiently.

Our team **"The New Way"** secured **4th place** in the competition.

---

## Competition Challenge
Micromouse is a maze navigation challenge where the robot must autonomously solve an unknown maze in the shortest possible time.

### Maze Specifications
- Maze path width: **10 cm**
- Robot dimensions: **8 × 8 cm**

---

## Hardware Components

- **STM32 Blue Pill (ARM Cortex-M3)**
- **3 × VL53L0X ToF Laser Distance Sensors**
- **TB6612 Motor Driver**
- **2 × N20 200 RPM 6V DC Motors with Encoders**
- **Li-ion Battery**
- **Buck Converter**
- **TTL Converter**
- **ST-Link Programmer**

---

## Features

- Autonomous maze navigation
- Left-wall-following maze solving logic
- Real-time obstacle detection
- Encoder-based turning control
- Precise 90-degree turns
- Dead-end detection and recovery
- Multi-ToF sensor wall sensing
- STM32 embedded control implementation

---

## System Working

### 1. Wall Detection
The robot uses three **VL53L0X Time-of-Flight sensors**:

- Left sensor
- Center sensor
- Right sensor

These continuously measure distances to nearby walls.

---

### 2. Forward Navigation
During movement:

- The robot follows the maze using wall-following logic
- Motor speed is dynamically adjusted based on wall distance
- If too close to a wall, speed correction is applied

---

### 3. Maze Decision Logic
The navigation follows this priority:

**Left → Straight → Right → Back**

Decision process:

- If left path is open → turn left
- Else if front is open → continue straight
- Else if right is open → turn right
- Else → dead end → reverse and search for open path

---

### 4. Turning Control
90-degree turns are performed using:

- Wheel encoder feedback
- Tick counting for controlled rotation

This improves turning consistency compared to time-based turning.

---

### 5. Dead-End Recovery
If all directions are blocked:

- Robot moves backward
- Continuously scans for openings
- Turns toward newly detected open path

---

## Software Implementation

### Language
- C++

### Communication Protocols
- I2C (ToF sensors)
- UART (Bluetooth debugging / serial monitoring)

### Core Concepts Used
- Sensor fusion
- Encoder interrupts
- Wall following algorithm
- State-based navigation logic
- Embedded motor control

---

## Key Learnings
This project helped us gain practical experience with **STM32 ARM Cortex microcontrollers**, moving beyond Arduino and ESP platforms.

We learned:

- Flash memory programming
- UART code uploading using TTL converter
- ST-Link debugging
- I2C multi-device addressing
- Encoder interrupt handling
- Real-time motor control
- Embedded hardware debugging

---

## Challenges Faced
During development, we faced several hardware debugging challenges:

- Damaged a ToF sensor due to improper soldering
- Damaged an ST-Link due to incorrect pin swapping during debugging
- Competition penalties affected final ranking despite fast maze solving

These experiences taught us patience, hardware discipline, and debugging precision.

---

## Achievement
🏅 **4th Place – Micromouse Competition**

Although the robot solved the maze efficiently, penalties prevented a higher ranking.

---

## Future Improvements
- PID wall following
- Faster optimized path replay
- Maze mapping memory
- Shortest path solving algorithm
- Speed optimization
- Better turn calibration

---

## Team
**The New Way**
