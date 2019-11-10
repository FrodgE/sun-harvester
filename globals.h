#ifndef globals_h
#define globals_h

#include "configuration.h"


extern bool FirstIterationAfterArduinoReset;
extern bool joystickMode;
extern bool justFinishedManualControl;

extern int calculationScale;
extern int iterationsAfterReset;
extern int machineNumber;
extern int targetsUsed;

extern float altManualSpeedSwap;
extern float altMove;
extern float azManualSpeedSwap;
extern float azMove;
extern float pi;
extern float SunsAltitude;
extern float SunsAzimuth;

extern float altLeftoverSteps[];
extern float azLeftoverSteps[];
extern float MachinesPrevAlt[];
extern float MachinesPrevAz[];
extern float MachineTargetAlt[];
extern float MachineTargetAz[];

extern calculationSpeed_t calculationSpeed;

#endif
