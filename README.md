# Universal Arduino Garage Door Controller

A universal garage door controller built using an Arduino Nano microcontroller. In operattion since january 2024 without problems.
The system controls a garage door motor using a single pushbutton and two limit switches while implementing several safety mechanisms to protect the motor, hardware and people.

The project was originally developed for a Wayne Dalton garage door with an ecostar electric lift mechanism but can be adapted for many similar garage door systems.

---

## Features

* Single-button garage door operation
* Direction change during movement
* Ability to stop the door mid-movement
* Automatic stop at upper and lower limit switches
* Motor overcurrent protection
* Movement timeout protection
* Calibration of current sensor offset
* Noise-resistant button handling

---

## System Behaviour

The controller behaves similarly to commercial garage door openers:

1. If the door is **closed**, pressing the button opens it.
2. If the door is **open**, pressing the button closes it.
3. If the door is **moving**, pressing the button stops it.
4. If the door is **stopped between limits**, pressing the button reverses the previous direction.

---

## Hardware

Controller:

* Arduino Nano

Inputs:

* Pushbutton
* Upper limit switch
* Lower limit switch
* ACS current sensor

Outputs:

* Two relays controlling motor direction

---

## Pin Configuration (example)

| Component          | Arduino Pin |
| ------------------ | ----------- |
| Button             | D5          |
| Lower limit switch | D2          |
| Upper limit switch | D11         |
| Relay 1            | D8          |
| Relay 2            | D7          |
| Current sensor     | A0          |

---

## Safety Mechanisms

### Movement Timeout

The controller stops the motor if the movement exceeds a predefined time limit.
This protects the system if a limit switch fails or the door becomes blocked.

---

### Limit Switch Validation

If both limit switches are detected as active simultaneously, the system assumes a malfunction and stops the motor.

---

### Motor Overcurrent Protection

The motor current is monitored using an ACS current sensor.

If the measured current exceeds a defined threshold for multiple measurement intervals, the motor is stopped to prevent damage.

---

### Button Noise Filtering

The button must be held for a minimum duration before the controller accepts it as a valid press.
This prevents accidental triggering caused by electrical noise.

---

## Configuration Parameters

The following constants can be adjusted to match a specific garage door system:

| Parameter           | Description                           |
| ------------------- | ------------------------------------- |
| `maxMoveTime`       | Maximum downward movement time        |
| `upperMoveExtra`    | Extra time buffer for upward movement |
| `maxCurrent`        | Maximum allowed motor current         |
| `requiredPressTime` | Minimum button press duration         |
| `sensitivity`       | Current sensor sensitivity            |

---

## Installation

1. Connect the hardware according to the wiring diagram.
2. Upload the Arduino sketch to the controller.
3. Power the system.
4. Test door movement carefully.

⚠️ **Important:**
During the first test verify that the motor direction matches the expected direction.

If the door moves opposite to the expected direction, **swap the motor polarity**.

---

## Repository Structure

```
src/
    garage_door_controller.ino

docs/
    wiring_diagram.png
    installed_system.jpg
```

---

## Disclaimer

⚠️ **Important:**
This project may require working with 230 VAC systems, which should only be done by professionals!
Improper wiring or configuration may cause equipment damage or personal injury.

Use at your own risk and always test the system carefully.

---

## License

This project is released under the MIT License.
