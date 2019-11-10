#ifndef configuration_h
#define configuration_h

#include "types.h"

#include <Arduino.h>


//Put how many machines you want to control here
#define numberOfMachines 1

extern const char latitude[];
extern const char longitude[];
extern const bool useNorthAsZero;
extern const float timezone;
extern const unsigned long updateEvery;
extern const int hourReset;

extern const float moveAwayFromLimit;
extern const float machineAltParkAngle;
extern const float machineAzParkAngle;

extern const bool potentiometerIsConnected;

extern const float steps;
extern const float altSpeed;
extern const float azSpeed;
extern const float altResetSpeed;
extern const float azResetSpeed;
extern float altManualSpeed;
extern float azManualSpeed;
extern const activeHighLow_t enableHIGHorLOW;

extern const float altAccel;
extern const float azAccel;
extern const long altitudeMax;
extern const long azimuthMax;

extern const int azimuthStepPin;
extern const int azimuthDirPin;
extern const int altitudeStepPin;
extern const int altitudeDirPin;
extern const int EnablePin;
extern const int altLimitPin;
extern const int azLimitPin;
extern const int manualModeOnOffPin;
extern const int WindProtectionSwitch;
extern const int HeliostatToSun;
extern const int joystickUDAnalogPin;
extern const int joystickLRAnalogPin;
extern const int invertUD;
extern const int invertLR;
extern const int plusOneButton;
extern const int minusOneButton;

extern const float MachineSettings[][20] PROGMEM;

#endif
