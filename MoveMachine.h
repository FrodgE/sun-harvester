#ifndef movemachine_h
#define movemachine_h

#include "types.h"

void moveMachine(float preTargetAlt, float preTargetAz, float targetalt, float targetaz, machineType_t sunTrackerOrHelio, float altGearRatio, float altMotorDirection, float altb, float altc, float altAngleAtZero, linearAngle_t altAcuteObtuse, float altLimitAngle, float azGearRatio, float azMotorDirection, float azb, float azc, float azAngleAtZero, linearAngle_t azAcuteObtuse, float azLimitAngle, float minAz, float minAlt, float maxAz, float maxAlt);

#endif
