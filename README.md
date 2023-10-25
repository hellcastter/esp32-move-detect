# ESP32: Detect movements
Detect movements on ESP32 using camera

## Team
Mentors:
- Danylo Sahaidak
- Bohdan Yavorivskiy

Members:
- Dmytro Shumskyi
- Yurii Sahaidak
- Marta Samoilenko
- Victor Muryn

## Used components
- Board ESP32 (one of these):
  - ESP-WROOM 32
  - ESP32-CAM
- Camera OV7670

## Installation
Project uses esp32-camera library, so use this command to install library.
```bash
git clone --recursive git@github.com:hellcastter/esp32-move-detect.git
```

## Settings
Open <i>main/camera.c</i> and select your board and configure PSRAM as it mentioned there.

## Upload
After that you are ready to go. Build and flash code to your board using interface of Esp-IDF in VS Code 
or use command line interface of idf.py in your terminal.
```bash
idf.py build
idf.py -p PORT flash
```
