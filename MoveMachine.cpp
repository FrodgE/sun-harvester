#include "MoveMachine.h"
#include "Functions.h"
#include "Linear_Actuator_Code.h"
#include "globals.h"

#include <Arduino.h>

long GearRatioMoveMotor(altAz_t altOrAz, float MachinesPreviousAngle, float MachinesNewAngle, float GearRatio, float MotorDirection);
void resetPositionOfMachine(altAziAssy_t assemblyType, float altMotorDirection, float altLimitAngle, float altGearRatio, float altb, float altc, linearAngle_t altAcuteObtuse, float altAngleAtZero, float azMotorDirection, float azLimitAngle, float azGearRatio, float azb, float azc, linearAngle_t azAcuteObtuse, float azAngleAtZero);
void gearReductionReset(altAz_t altOrAz, float MotorDirection, float LimitAngle, float GearRatio);

void moveMachine(float preTargetAlt, float preTargetAz, float targetalt, float targetaz, altAziAssy_t assemblyType, machineType_t sunTrackerOrHelio, float altGearRatio, float altMotorDirection, float altb, float altc, float altAngleAtZero, linearAngle_t altAcuteObtuse, float altLimitAngle, float azGearRatio, float azMotorDirection, float azb, float azc, float azAngleAtZero, linearAngle_t azAcuteObtuse, float azLimitAngle, float minAz, float minAlt, float maxAz, float maxAlt)
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
            if (joystickMode == false) {
                if (FirstIterationAfterArduinoReset == true) {

                    Serial.print("Target's Alt: ");
                    Serial.println(MachineTargetAlt[machineNumber], 3);
                    delay(50);
                    Serial.print("Target's Az: ");
                    Serial.println(MachineTargetAz[machineNumber], 3);
                    delay(50);
                }
            }
        }

        if (digitalRead(WindProtectionSwitch) == HIGH) {
            MachinesNewAltitude = machineAltParkAngle;
            MachinesNewAzimuth = machineAzParkAngle;
        }

        if (joystickMode == false) {
            Serial.print("Machine's Alt: ");
            Serial.println(MachinesNewAltitude, 3);
            delay(50);
            Serial.print("Machine's Az: ");
            Serial.println(MachinesNewAzimuth, 3);
            delay(50);
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

                    moveToPosition(true, altsteps, azsteps);
                }

                MachinesPrevAlt[machineNumber] = MachinesNewAltitude;
                MachinesPrevAz[machineNumber] = MachinesNewAzimuth;
            } //if (iterationsAfterReset > 1)
        } else {
            Serial.println("Move exceeds bounds");
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
long GearRatioMoveMotor(altAz_t altOrAz, float MachinesPreviousAngle, float MachinesNewAngle, float GearRatio, float MotorDirection)
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
void resetPositionOfMachine(altAziAssy_t assemblyType, float altMotorDirection, float altLimitAngle, float altGearRatio, float altb, float altc, linearAngle_t altAcuteObtuse, float altAngleAtZero, float azMotorDirection, float azLimitAngle, float azGearRatio, float azb, float azc, linearAngle_t azAcuteObtuse, float azAngleAtZero)
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
void gearReductionReset(altAz_t altOrAz, float MotorDirection, float LimitAngle, float GearRatio)
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
