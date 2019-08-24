#ifndef globals_h
#define globals_h

#include "arduino.h"

//Put how many machines you want to control here
#define numberOfMachines 1


extern int altitudeDirPin;
extern int altitudeStepPin;
extern int azimuthDirPin;
extern int azimuthStepPin;
extern int calculationScale;
extern int calculationSpeed;
extern int enableHIGHorLOW;
extern int EnablePin;
extern int FirstIterationAfterArduinoReset;
extern int invertLR;
extern int invertUD;
extern int iterationsAfterReset;
extern int joystickLRAnalogPin;
extern int joystickModeOnOff;
extern int joystickTriggerOnce;
extern int joystickUDAnalogPin;
extern int justFinishedManualControl;
extern int machineNumber;
extern int manualMachineNumber;
extern int minusOneButton;
extern int plusOneButton;
extern int potentiometerIsConnected;
extern int targetsUsed;

extern const int altLimitPin;
extern const int azLimitPin;
extern const int HeliostatToSun;
extern const int manualModeOnOffPin;
extern const int WindProtectionSwitch;

extern long altitudeMax;
extern long azimuthMax;

extern float altAccel;
extern float altManualSpeed;
extern float altManualSpeedSwap;
extern float altMove;
extern float altResetSpeed;
extern float altSpeed;
extern float azAccel;
extern float azManualSpeed;
extern float azManualSpeedSwap;
extern float azMove;
extern float azResetSpeed;
extern float azSpeed;
extern float LRCenter;
extern float machineAltParkAngle;
extern float machineAzParkAngle;
extern float moveAwayFromLimit;
extern float pi;
extern float steps;
extern float SunsAltitude;
extern float SunsAzimuth;
extern float UDCenter;

extern float altLeftoverSteps[];
extern float azLeftoverSteps[];
extern float MachinesPrevAlt[];
extern float MachinesPrevAz[];
extern float MachineTargetAlt[];
extern float MachineTargetAz[];

extern const PROGMEM int HourAngleArray[];
extern const float MachineSettings[][20] PROGMEM;

#endif
