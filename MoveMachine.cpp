#include "MoveMachine.h"
#include "Functions.h"
#include "Linear_Actuator_Code.h"
#include "globals.h"

#include <Arduino.h>

long GearRatioMoveMotor(const altAz_t &altOrAz, const float &MachinesPreviousAngle, const float &MachinesNewAngle, const float &GearRatio, const float &MotorDirection);
void resetPositionOfMachine(const altAziAssy_t &assemblyType, const float &altMotorDirection, const float &altLimitAngle, const float &altGearRatio, const float &altb, const float &altc, const linearAngle_t &altAcuteObtuse, const float &altAngleAtZero, const float &azMotorDirection, const float &azLimitAngle, const float &azGearRatio, const float &azb, const float &azc, const linearAngle_t &azAcuteObtuse, const float &azAngleAtZero);
void gearReductionReset(const altAz_t &altOrAz, const float &MotorDirection, const float &LimitAngle, const float &GearRatio);

void moveMachine(const float &preTargetAlt, const float &preTargetAz, const float &targetalt, const float &targetaz, const altAziAssy_t &assemblyType, machineType_t sunTrackerOrHelio, const float &altGearRatio, const float &altMotorDirection, const float &altb, const float &altc, const float &altAngleAtZero, const linearAngle_t &altAcuteObtuse, const float &altLimitAngle, const float &azGearRatio, const float &azMotorDirection, const float &azb, const float &azc, const float &azAngleAtZero, const linearAngle_t &azAcuteObtuse, const float &azLimitAngle, const float &minAz, const float &minAlt, const float &maxAz, const float &maxAlt)
{
    if (digitalRead(HeliostatToSun) == HIGH) {
        sunTrackerOrHelio = SUN_TRACKER;
    }

    if ((iterationsAfterReset == 0) || (FirstIterationAfterArduinoReset == false)) { //Machine Resets Position
        if ((digitalRead(WindProtectionSwitch) != HIGH)) {
            Serial.println("Resetting");
            MachineOn(machineNumber);
            resetPositionOfMachine(assemblyType, altMotorDirection, altLimitAngle, altGearRatio, altb, altc, altAcuteObtuse, altAngleAtZero,
                azMotorDirection, azLimitAngle, azGearRatio, azb, azc, azAcuteObtuse, azAngleAtZero);
        }
    }

    if (iterationsAfterReset == 1) { //Machine has just finished resetting position
        MachinesPrevAlt[machineNumber] = positionAfterReset(altLimitAngle);
        MachinesPrevAz[machineNumber] = positionAfterReset(azLimitAngle);
    }

    float MachinesNewAltitude, MachinesNewAzimuth, altsteps, azsteps;
    float MachinesPreviousAltitude = MachinesPrevAlt[machineNumber];
    float MachinesPreviousAzimuth = MachinesPrevAz[machineNumber];

    if (SunsAltitude >= 0 || digitalRead(WindProtectionSwitch) == HIGH || digitalRead(manualModeOnOffPin) == HIGH) { //Waits until the sun is above the horizon before moving normally.
        //This code will also run if either manual mode or wind protection mode is activated.
        if (sunTrackerOrHelio == SUN_TRACKER) { //Machine Acts as Dual Axis Sun Tracker
            MachinesNewAltitude = SunsAltitude;
            MachinesNewAzimuth = SunsAzimuth;
        }

        if (sunTrackerOrHelio == HELIOSTAT) { //Machine Acts as Heliostat
            FindHeliostatAltAndAz(SunsAltitude, SunsAzimuth, targetalt, targetaz, MachinesNewAltitude, MachinesNewAzimuth);
        }

        if (digitalRead(WindProtectionSwitch) == HIGH) {
            MachinesNewAltitude = machineAltParkAngle;
            MachinesNewAzimuth = machineAzParkAngle;
        }

        //MACHINE ONLY MOVES WHEN REQUIRED POSITION IS WITHIN MACHINE'S LIMITS
        if ((MachinesNewAzimuth > minAz) && (MachinesNewAltitude > minAlt) && (MachinesNewAzimuth < maxAz) && (MachinesNewAltitude < maxAlt)) {
            if (iterationsAfterReset > 0) { //This code does not run directly after machine resets.

                //Machine uses Gear Reduction System for both AZIMUTH and ALTITUDE
                if (assemblyType == ALT_GEAR_AZI_GEAR) {
                    azsteps = GearRatioMoveMotor(AZIMUTH, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection);
                    altsteps = GearRatioMoveMotor(ALTITUDE, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection);
                }

                //Machine uses Linear Actuator for both AZIMUTH and ALTITUDE
                if (assemblyType == ALT_LINEAR_AZI_LINEAR) {
                    azsteps = linearActuatorMoveMotor(AZIMUTH, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection, azb, azc, azAcuteObtuse, azAngleAtZero);
                    altsteps = linearActuatorMoveMotor(ALTITUDE, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection, altb, altc, altAcuteObtuse, altAngleAtZero);
                }

                //Machine uses Linear Actuator for ALTITUDE and Gear Reduction System for AZIMUTH
                if (assemblyType == ALT_LINEAR_AZI_GEAR) {
                    azsteps = GearRatioMoveMotor(AZIMUTH, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection);
                    altsteps = linearActuatorMoveMotor(ALTITUDE, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection, altb, altc, altAcuteObtuse, altAngleAtZero);
                }

                //Machine uses Linear Actuator for AZIMUTH and Gear Reduction System for ALTITUDE
                if (assemblyType == ALT_GEAR_AZI_LINEAR) {
                    azsteps = linearActuatorMoveMotor(AZIMUTH, MachinesPreviousAzimuth, MachinesNewAzimuth, azGearRatio, azMotorDirection, azb, azc, azAcuteObtuse, azAngleAtZero);
                    altsteps = GearRatioMoveMotor(ALTITUDE, MachinesPreviousAltitude, MachinesNewAltitude, altGearRatio, altMotorDirection);
                }

                if (abs(altsteps) > 0 || abs(azsteps) > 0) {
                    if (joystickMode == false) {
                        MachineOn(machineNumber);
                    }

                    Serial.print("Machine Number ");
                    Serial.println(machineNumber);

                    Serial.print("Current Target Group: ");
                    Serial.println(targetsUsed);

                    if (joystickMode == false) {
                        if (sunTrackerOrHelio == HELIOSTAT) { //Machine Acts as Heliostat
                            if (FirstIterationAfterArduinoReset == true) {
                                Serial.print("Target's Alt: ");
                                Serial.println(MachineTargetAlt[machineNumber], 3);
                                Serial.print("Target's Az: ");
                                Serial.println(MachineTargetAz[machineNumber], 3);
                            }
                        }

                        Serial.print("Machine's Alt: ");
                        Serial.println(MachinesNewAltitude, 3);
                        Serial.print("Machine's Az: ");
                        Serial.println(MachinesNewAzimuth, 3);
                    }

                    moveToPosition(true, altsteps, azsteps);
                }

                MachinesPrevAlt[machineNumber] = MachinesNewAltitude;
                MachinesPrevAz[machineNumber] = MachinesNewAzimuth;
            } //if (iterationsAfterReset > 1)
        } else {
            Serial.print("Machine Number ");
            Serial.println(machineNumber);
            Serial.println("Move exceeds bounds");
            if ((MachinesNewAzimuth <= minAz) || (MachinesNewAzimuth >= maxAz)) {
                Serial.print("Attempted azimuth ");
                Serial.println(MachinesNewAzimuth);
                Serial.print("Machine minimum azimuth ");
                Serial.println(minAz);
                Serial.print("Machine maximum azimuth ");
                Serial.println(maxAz);
            }
            if ((MachinesNewAltitude <= minAlt) || (MachinesNewAltitude >= maxAlt)) {
                Serial.print("Attempted altitude ");
                Serial.println(MachinesNewAltitude);
                Serial.print("Machine minimum altitude ");
                Serial.println(minAlt);
                Serial.print("Machine maximum altitude ");
                Serial.println(maxAlt);
            }
            if (joystickMode == true) {
                if (sunTrackerOrHelio == SUN_TRACKER) {
                    SunsAltitude = MachinesPrevAlt[machineNumber] + altMove;
                    SunsAzimuth = MachinesPrevAz[machineNumber] + azMove;
                }

                if (sunTrackerOrHelio == HELIOSTAT) {
                    MachineTargetAlt[machineNumber] = MachineTargetAlt[machineNumber] + altMove;
                    MachineTargetAz[machineNumber] = MachineTargetAz[machineNumber] + azMove;
                }
            }
        } //END MACHINE ONLY MOVES WHEN REQUIRED POSITION IS WITHIN MACHINE'S LIMITS
    } //END (SunsAltitude>=0 || digitalRead(WindProtectionSwitch)==HIGH || digitalRead(manualModeOnOffPin)==HIGH)
    if (joystickMode == false) {
        MachineOff(machineNumber);
    }
} //END MOVE MACHINE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long GearRatioMoveMotor(const altAz_t &altOrAz, const float &MachinesPreviousAngle, const float &MachinesNewAngle, const float &GearRatio, const float &MotorDirection)
{
    float NumberOfSteps;
    if (altOrAz == ALTITUDE) {
        NumberOfSteps = -1 * steps * ((MachinesNewAngle - MachinesPreviousAngle) * GearRatio) / 360.0 * MotorDirection + altLeftoverSteps[machineNumber];
        if (abs(NumberOfSteps) == NumberOfSteps) {
            altLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps))));
        } else {
            altLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps)))) * -1;
        }
    }
    if (altOrAz == AZIMUTH) {
        NumberOfSteps = -1 * steps * ((MachinesNewAngle - MachinesPreviousAngle) * GearRatio) / 360.0 * MotorDirection + azLeftoverSteps[machineNumber];
        if (abs(NumberOfSteps) == NumberOfSteps) {
            azLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps))));
        } else {
            azLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps)))) * -1;
        }
    }
    return NumberOfSteps;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetPositionOfMachine(const altAziAssy_t &assemblyType, const float &altMotorDirection, const float &altLimitAngle, const float &altGearRatio, const float &altb, const float &altc, const linearAngle_t &altAcuteObtuse, const float &altAngleAtZero, const float &azMotorDirection, const float &azLimitAngle, const float &azGearRatio, const float &azb, const float &azc, const linearAngle_t &azAcuteObtuse, const float &azAngleAtZero)
{
    if (assemblyType == ALT_GEAR_AZI_GEAR) { //Machine uses Gear Reduction System for both AZIMUTH and ALTITUDE
        gearReductionReset(AZIMUTH, azMotorDirection, azLimitAngle, azGearRatio);
        gearReductionReset(ALTITUDE, altMotorDirection, altLimitAngle, altGearRatio);
    }

    if (assemblyType == ALT_LINEAR_AZI_LINEAR) { //Machine uses Linear Actuator for both AZIMUTH and ALTITUDE
        linearActuatorReset(AZIMUTH, azMotorDirection, azLimitAngle, azGearRatio, azb, azc, azAcuteObtuse, azAngleAtZero);
        linearActuatorReset(ALTITUDE, altMotorDirection, altLimitAngle, altGearRatio, altb, altc, altAcuteObtuse, altAngleAtZero);
    }

    if (assemblyType == ALT_LINEAR_AZI_GEAR) { //Machine uses Linear Actuator for ALTITUDE and Gear Reduction System for AZIMUTH
        gearReductionReset(AZIMUTH, azMotorDirection, azLimitAngle, azGearRatio);
        linearActuatorReset(ALTITUDE, altMotorDirection, altLimitAngle, altGearRatio, altb, altc, altAcuteObtuse, altAngleAtZero);
    }

    if (assemblyType == ALT_GEAR_AZI_LINEAR) { //Machine uses Linear Actuator for AZIMUTH and Gear Reduction System for ALTITUDE
        linearActuatorReset(AZIMUTH, azMotorDirection, azLimitAngle, azGearRatio, azb, azc, azAcuteObtuse, azAngleAtZero);
        gearReductionReset(ALTITUDE, altMotorDirection, altLimitAngle, altGearRatio);
    }
    MachinesPrevAlt[machineNumber] = positionAfterReset(altLimitAngle);
    MachinesPrevAz[machineNumber] = positionAfterReset(azLimitAngle);
    //Serial.println("Reset Finished");
} //END reset

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gearReductionReset(const altAz_t &altOrAz, const float &MotorDirection, const float &LimitAngle, const float &GearRatio)
{
    findLimits(altOrAz, MotorDirection, LimitAngle); //Seeks out limit switch
    float dif = positionAfterReset(LimitAngle) - LimitAngle;
    float NumberOfSteps = -1 * steps * (dif * GearRatio) / 360.0 * MotorDirection;
    if (altOrAz == AZIMUTH) {
        moveToPosition(true, 0, (NumberOfSteps));
    } //Moves motor away from limit switch
    if (altOrAz == ALTITUDE) {
        moveToPosition(true, (NumberOfSteps), 0);
    } //Moves motor away from limit switch
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
