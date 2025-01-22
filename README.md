

# **Temperature-Initiated Object Detection**

This repository contains the implementation of a **Temperature-Initiated Object Detection System** using the TM4C123G microcontroller. The project combines temperature sensing, object detection, and a user interface into a multi-functional system, integrating various utility modules.

---

## **Project Overview**

The system features:
- **Deep Sleep Mode**: Monitors temperature using an analog sensor (LM35) and wakes up on threshold exceedance.
- **High-Precision Heat Measurement**: Uses a digital temperature sensor (BMP280) for precise temperature readings.
- **Object Detection**: Scans the environment using an ultrasonic sensor mounted on a stepper motor.
- **User Interface**: Displays information on a Nokia 5110 LCD and uses RGB LEDs, a speaker, and pushbuttons for additional feedback.

---

## **Key Components**

### **1. Sensors**
- **LM35 Analog Temperature Sensor**: Monitors temperature in deep sleep.
- **BMP280 Digital Sensor**: Provides precise temperature readings using the I2C protocol.
- **HC-SR04 Ultrasonic Distance Sensor**: Detects objects within a 1-meter range.

### **2. Actuators**
- **Stepper Motor**: Rotates the ultrasonic sensor between -90° and +90° for scanning.
- **RGB LEDs**: Indicates object distance using color coding.
- **Speaker**: Provides auditory feedback when thresholds are exceeded.
- **Power LED**: Turns on during active mode for visual indication.

### **3. User Interface**
- **Nokia 5110 LCD**: Displays temperature, object distance, and angle measurements.
- **Trimpot**: Adjusts the analog temperature threshold.
- **4x4 Keypad**: Adjusts the digital temperature threshold.
- **Pushbutton**: Manually enters deep sleep mode.

---

## **System Features**

### **1. Heat Sensing in Deep Sleep Mode**
- Monitors temperature using the LM35 sensor via the analog comparator.
- Wakes the system on temperature rise above the threshold.
- Performs precise temperature measurement with BMP280 after waking.

### **2. Object Detection**
- If the BMP280 confirms high temperature, the system scans the environment (-90° to +90°).
- Displays distance and angle of detected objects.
- Uses RGB LEDs to indicate object distance:
  - **Green**: 75–100 cm.
  - **Blue**: 50–75 cm.
  - **Red**: <50 cm.

### **3. User Feedback**
- Displays measurements and system status on the LCD.
- Speaker plays a 2-second sound if high temperature is detected.
- Power LED remains on during active mode.

---

## **Hardware Components**
| Component             | Function                                         |
|-----------------------|-------------------------------------------------|
| **TM4C123G**          | Microcontroller for system control.             |
| **Nokia 5110 LCD**    | Displays system information.                    |
| **HC-SR04**           | Ultrasonic distance measurement.                |
| **LM35**              | Analog temperature sensing.                     |
| **BMP280**            | Precise digital temperature measurement.        |
| **Stepper Motor**     | Rotates the ultrasonic sensor for scanning.     |
| **RGB LEDs**          | Indicates object distance.                      |
| **Speaker**           | Auditory feedback for high temperature.         |
| **Trimpot**           | Analog temperature threshold adjustment.        |
| **4x4 Keypad**        | Digital threshold adjustment.                   |
| **Power LED**         | Visual indicator during active mode.            |

---

## **Programming Requirements**
- **ARM Assembly**: Only keyboard functions implemented in assembly language.
- **C Programming**: Remaining modules written in C.
- **Interrupt Handling**:
  - Analog comparator interrupts for deep sleep wakeup.
  - I2C interrupts for BMP280 readings.
  - GPTM interrupts for ultrasonic sensor control.

---

## **How to Run**

### **1. Hardware Setup**
- Connect all components as per the project requirements.
- Use external power for high-current components (Power LED and speaker).

### **2. Software Setup**
- Install Keil µVision IDE for ARM development.
- Clone this repository:
  ```bash
  git clone https://github.com/sertacsatiroglu/temperature-object-detection.git
  ```
- Open the project in Keil µVision and build the solution.

### **3. Testing**
- Upload the program to the TM4C123G microcontroller.
- Simulate temperature changes using a lighter or equivalent heat source.
- Verify object detection using objects within a 1-meter range.

---

## **Deliverables**

### **1. Code**
- Fully executable code with well-commented sections.
- Includes ARM assembly for at least one submodule.

### **2. Final Report**
- Contains detailed descriptions of implementation, hardware photos, and relevant code snippets.

### **3. Final Video**
- Demonstrates the system's functionality (maximum 5 minutes).

### **4. Lab Demo**
- Real-time demonstration of the system during the lab session.

---

## **Contributing**
Contributions to improve or extend this project are welcome. Follow these steps:
1. Fork the repository.
2. Create a new branch for your feature or fix.
3. Submit a pull request with a clear description of changes.

---

## **License**
This project is open for academic and non-commercial use. For other purposes, please contact the repository owner.

---

## **References**
1. [TM4C123G Datasheet](http://www.ti.com/lit/ds/spms376e/spms376e.pdf)
2. [Valvano's Starter Files](https://users.ece.utexas.edu/~valvano/arm/)

---

