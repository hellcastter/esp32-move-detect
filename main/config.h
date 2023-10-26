//
// Created by Victor Muryn on 26.10.2023.
//

#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

// Chose your camera
//#define BOARD_WROVER_KIT 1
//#define BOARD_ESP32CAM_AITHINKER 1

#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 32
#define CAM_PIN_SIOD 21
#define CAM_PIN_SIOC 22

#define CAM_PIN_D7 4
#define CAM_PIN_D6 12
#define CAM_PIN_D5 13
#define CAM_PIN_D4 14
#define CAM_PIN_D3 15
#define CAM_PIN_D2 16
#define CAM_PIN_D1 17
#define CAM_PIN_D0 27

#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 35
#define CAM_PIN_PCLK 33

//#endif
#else
// ESP32Cam (AiThinker) PIN Map
//#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

#endif //CAMERA_CONFIG_H
