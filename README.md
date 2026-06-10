**434Mhz RF Wireless 4-Button Remote LED Control System**

## Project Overview

This project demonstrates a wireless communication system using 433MHz RF Transmitter and Receiver modules with Arduino UNO.

The transmitter unit contains four push buttons (A, B, C, and D). Each button sends a unique RF code through the 433MHz RF transmitter module.

The receiver unit consists of an Arduino UNO, a 433MHz RF receiver module, and four LEDs. When a valid RF code is received, the Arduino decodes the signal and toggles the corresponding LED.

The system enables wireless control of multiple devices using a simple RF remote and can be extended for home automation, wireless switching, and embedded system applications.

---

## Features

* 433MHz Wireless RF Communication
* 4 Independent Push Buttons
* 4 Independent LED Outputs
* Interrupt-Based RF Signal Decoding
* EEPROM Storage Support
* Wireless ON/OFF Control
* Low Cost Implementation
* Easy Hardware Setup
* Expandable Design

---

## System Architecture

Transmitter Section

Button A → RF Code A

Button B → RF Code B

Button C → RF Code C

Button D → RF Code D

↓

433MHz RF Transmitter

↓

Wireless Communication

↓

433MHz RF Receiver

↓

Arduino UNO

↓

LED Control Logic

↓

LED1 / LED2 / LED3 / LED4

---

## Working Principle

### Button A

When Button A is pressed on the transmitter side, RF Code A is transmitted through the 433MHz RF module.

The receiver receives and decodes the signal.

If the received code matches Code A, LED1 toggles ON/OFF.

---

### Button B

When Button B is pressed, RF Code B is transmitted.

The receiver decodes the received signal.

If the received code matches Code B, LED2 toggles ON/OFF.

---

### Button C

When Button C is pressed, RF Code C is transmitted.

The receiver verifies the received RF code.

If the received code matches Code C, LED3 toggles ON/OFF.

---

### Button D

When Button D is pressed, RF Code D is transmitted.

The receiver decodes the signal.

If the received code matches Code D, LED4 toggles ON/OFF.

---

## Hardware Components

| Component                    | Quantity    |
| ---------------------------- | ----------- |
| Arduino UNO                  | 2           |
| 433MHz RF Transmitter Module | 1           |
| 433MHz RF Receiver Module    | 1           |
| Push Buttons                 | 4           |
| LEDs                         | 4           |
| 220Ω Resistors               | 4           |
| Breadboard                   | 2           |
| Jumper Wires                 | As Required |
| USB Cable                    | 2           |

---

## Transmitter Connections

| Component  | Arduino Pin |
| ---------- | ----------- |
| Button A   | D4          |
| Button B   | D5          |
| Button C   | D6          |
| Button D   | D7          |
| RF TX DATA | D12         |
| VCC        | 5V          |
| GND        | GND         |

---

## Receiver Connections

| Component  | Arduino Pin |
| ---------- | ----------- |
| RF RX DATA | D2 (INT0)   |
| LED1       | D13         |
| LED2       | D12         |
| LED3       | D11         |
| LED4       | D10         |
| VCC        | 5V          |
| GND        | GND         |

---

## Software Requirements

* Arduino IDE
* Arduino UNO Board Package
* USB Drivers
* GitHub Account

---

## Project Flow

1. User presses a button on the RF transmitter.
2. Arduino transmitter generates a unique RF code.
3. 433MHz transmitter sends the code wirelessly.
4. 433MHz receiver captures the RF signal.
5. Arduino receiver decodes the received signal.
6. Received code is compared with stored button codes.
7. Matching LED output is selected.
8. Corresponding LED toggles ON/OFF.
9. System waits for the next RF command.

---

## Expected Output

| Button Pressed | Receiver Action |
| -------------- | --------------- |
| A              | LED1 Toggle     |
| B              | LED2 Toggle     |
| C              | LED3 Toggle     |
| D              | LED4 Toggle     |

---

## Example Output

Button A Pressed

LED1 ON

Button A Pressed Again

LED1 OFF

Button B Pressed

LED2 ON

Button B Pressed Again

LED2 OFF

Button C Pressed

LED3 ON

Button C Pressed Again

LED3 OFF

Button D Pressed

LED4 ON

Button D Pressed Again

LED4 OFF

---

##Hardware setup

<img width="554" height="696" alt="Screenshot 2026-06-05 182720" src="https://github.com/user-attachments/assets/1dc58787-5106-42e0-adf9-c1deb1c1dec6" />

<img width="1201" height="705" alt="Screenshot 2026-06-05 182033" src="https://github.com/user-attachments/assets/3766c4ac-58e0-4e2f-9d39-f75ea0778cca" />

##Output

<img width="839" height="1874" alt="image" src="https://github.com/user-attachments/assets/26452712-0941-4e14-8eba-cf89ba6a6773" />

##Demo video

https://drive.google.com/file/d/15iQQXlbIbQslru1QIPL0FEx4ntBPUvnE/view?usp=drivesdk

## Applications

* Wireless Light Control
* Home Automation
* Smart Switch Systems
* Remote Device Control
* Embedded System Projects
* Industrial Monitoring
* Educational Demonstration Projects
* IoT Prototyping

---

## Advantages

* Low Power Consumption
* Low Cost Hardware
* Wireless Operation
* Easy Installation
* Expandable Architecture
* Reliable Communication
* Beginner Friendly Project

---

## Future Enhancements

* Relay Control System
* Home Automation Integration
* Mobile App Interface
* LCD Display Support
* OLED Status Display
* Password Protected RF Access
* Multi-Room Wireless Control
* IoT Cloud Connectivity

---

## Folder Structure

433MHz-RF-Remote-Control/

│

├── Transmitter/

│ └── RF_Transmitter.ino

│

├── Receiver/

│ └── RF_Receiver.ino

│

├── Hardware/

│ ├── Transmitter_Setup.jpg

│ ├── Receiver_Setup.jpg

│ ├── Circuit_Diagram.png

│ └── System_Setup.jpg

│

├── Output/

│ ├── LED1_ON.jpg

│ ├── LED2_ON.jpg

│ ├── LED3_ON.jpg

│ ├── LED4_ON.jpg

│ └── Serial_Output.png

│

├── Demo/

│ └── Demo_Video.mp4

│

└── README.md

---

## Demo Video

The demonstration video shows:

* Complete hardware setup
* RF transmitter operation
* Wireless communication
* LED control functionality
* Real-time response of the receiver
* Button A, B, C, and D testing
* Full project demonstration

