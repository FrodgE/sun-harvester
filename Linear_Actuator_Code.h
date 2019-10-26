#ifndef linear_actuator_code_h
#define linear_actuator_code_h

#include "types.h"

long linearActuatorMoveMotor(const altAz_t &altOrAz, const float &MachinesPreviousAngle, const float &MachinesNewAngle, const float &GearRatio, const float &MotorDirection, const float &b, const float &c, const linearAngle_t &AcuteObtuse, const float &AngleAtZero);

void linearActuatorReset(const altAz_t &altOrAz, const float &MotorDirection, const float &LimitAngle, const float &GearRatio, const float &b, const float &c, const linearAngle_t &AcuteObtuse, const float &AngleAtZero);

#endif
