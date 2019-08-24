#ifndef linear_actuator_code_h
#define linear_actuator_code_h

long linearActuatorMoveMotor(int altOrAz, float MachinesPreviousAngle, float MachinesNewAngle, float GearRatio, float MotorDirection, float b, float c, int AcuteObtuse, float AngleAtZero);

void linearActuatorReset(int altOrAz, float MotorDirection, float LimitAngle, float GearRatio, float b, float c, float AcuteObtuse, float AngleAtZero);

#endif
