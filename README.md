# EEN1071 Assignment 2: Real-Time MQTT Data Visualisation

## Project Overview
This project is a **Qt-based GUI application** designed to monitor and visualise real-time telemetry data (such as CPU temperature or pitch) via the **MQTT (Message Queuing Telemetry Transport)** protocol. 

The application connects to an MQTT broker, subscribes to a specific topic, and plots incoming numerical data on a dynamic, real-time graph.
Please note that there is a sub folder ADXL345 which contains all of the code needed for the ADXL345 sensor.f

## Key Features
* **Real-Time Plotting:** Utilizes the `QCustomPlot` library to visualise data with a scrolling time-based X-axis.
* **Asynchronous Messaging:** Integrates the **Paho MQTT C Client** to handle background data reception.
* **Thread-Safe Architecture:** Uses Qt's Signal/Slot mechanism to bridge the gap between the MQTT callback thread and the GUI main thread.
* **Interactive Controls:** Manually adjust values or connect/disconnect from the broker via an intuitive interface.

## Technical Architecture

### Threading Model 
The application handles a common challenge in GUI development: **non-blocking I/O**. 
1. The **MQTT Library** runs its own background thread to listen for messages.
2. When a message arrives, a **C-style callback** (`msgarrvd`) is triggered.
3. To prevent UI crashes, this callback **emits a Qt Signal**, which is safely queued and processed by the **Main UI Thread** to update the graph.



## Prerequisites
To build and run this project, you need:
* **Qt Framework** (Version 5 or 6)
* **QCustomPlot** (Included in source)
* **Paho MQTT C Library** (`libpaho-mqtt3c`)
    * On Linux: `sudo apt-get install libpaho-mqtt-dev`
* **MQTT Broker:** An active broker (like Mosquitto) running at `127.0.0.1:1883`.

## Installation & Build
1. Clone the repository or extract the source files.
2. Open `EEN1071Ass2.pro` in **Qt Creator**.
3. Ensure the `LIBS += -lpaho-mqtt3c` line in the `.pro` file points to your installed library location.
4. Run `qmake` and then **Build**.

## Usage
1. **Connect:** Click the "Connect" button to link to the MQTT broker. Check the output text area for connection status.
2. **Monitor:** Once connected, the application subscribes to `een1071/CPUTemp`. Any incoming numerical payload will be plotted on the graph.
3. **Manual Test:** Use the "Up" and "Down" buttons to manually simulate data changes on the graph.
4. **Disconnect:** Use the "Disconnect" button to safely close the MQTT session.

## Project Structure
* `main.cpp`: Application entry point.
* `mainwindow.cpp/.h`: Core logic and MQTT integration.
* `qcustomplot.cpp/.h`: Third-party graphing library.
* `mainwindow.ui`: XML definition of the user interface.

---
*Developed as part of EEN1071 - Assignment 2.*
