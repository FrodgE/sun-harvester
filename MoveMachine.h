#ifndef movemachine_h
#define movemachine_h

#include "types.h"

void moveMachine(const float &preTargetAlt, const float &preTargetAz, const float &targetalt, const float &targetaz, const altAziAssy_t &assemblyType, machineType_t sunTrackerOrHelio, const float &altGearRatio, const float &altMotorDirection, const float &altb, const float &altc, const float &altAngleAtZero, const linearAngle_t &altAcuteObtuse, const float &altLimitAngle, const float &azGearRatio, const float &azMotorDirection, const float &azb, const float &azc, const float &azAngleAtZero, const linearAngle_t &azAcuteObtuse, const float &azLimitAngle, const float &minAz, const float &minAlt, const float &maxAz, const float &maxAlt);

#endif
