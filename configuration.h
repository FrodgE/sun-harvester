#ifndef configuration_h
#define configuration_h

#include "types.h"

#include <Arduino.h>


//Put how many machines you want to control here
#define numberOfMachines 1

extern const char latitude[];
extern const char longitude[];
extern bool useNorthAsZero;
extern float timezone;
extern unsigned long updateEvery;
extern int hourReset;

extern float moveAwayFromLimit;
extern float machineAltParkAngle;
extern float machineAzParkAngle;

extern bool potentiometerIsConnected;

extern float steps;
extern float altSpeed;
extern float azSpeed;
extern float altResetSpeed;
extern float azResetSpeed;
extern float altManualSpeed;
extern float azManualSpeed;
extern activeHighLow_t enableHIGHorLOW;

extern float altAccel;
extern float azAccel;
extern long altitudeMax;
extern long azimuthMax;

extern int azimuthStepPin;
extern int azimuthDirPin;
extern int altitudeStepPin;
extern int altitudeDirPin;
extern int EnablePin;
extern const int altLimitPin;
extern const int azLimitPin;
extern const int manualModeOnOffPin;
extern const int WindProtectionSwitch;
extern const int HeliostatToSun;
extern int joystickUDAnalogPin;
extern int joystickLRAnalogPin;
extern int invertUD;
extern int invertLR;
extern int plusOneButton;
extern int minusOneButton;

extern const float MachineSettings[][20] PROGMEM;

#endif
