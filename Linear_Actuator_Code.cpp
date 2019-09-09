#include "Linear_Actuator_Code.h"
#include "Functions.h"
#include "globals.h"

#include <Arduino.h>

float leadscrewLength(float b, float c, linearAngle_t AcuteObtuse, float angle, float AngleAtZero);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code calculates the length of the linear actuator.
float leadscrewLength(float b, float c, linearAngle_t AcuteObtuse, float angle, float AngleAtZero)
{
    float output;
    if (AcuteObtuse == ACUTE) {
        angle = AngleAtZero - angle;
    }
    if (AcuteObtuse == OBTUSE) {
        angle = AngleAtZero + angle;
    }
    output = sqrt(b * b + c * c - 2 * b * c * cos((angle)*pi / 180));
    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long linearActuatorMoveMotor(int altOrAz, float MachinesPreviousAngle, float MachinesNewAngle, float GearRatio, float MotorDirection, float b, float c, linearAngle_t AcuteObtuse, float AngleAtZero)
{
    float NewLength = leadscrewLength(b, c, AcuteObtuse, MachinesNewAngle, AngleAtZero); //New Leadscrew Length
    float PreviousLength = leadscrewLength(b, c, AcuteObtuse, MachinesPreviousAngle, AngleAtZero); //Previous Leadscrew Length
    float ChangeInLength = NewLength - PreviousLength;
    float NumberOfSteps;

    if (altOrAz == 1) {
        NumberOfSteps = steps * ChangeInLength * GearRatio * MotorDirection + altLeftoverSteps[machineNumber];
        if (abs(NumberOfSteps) == NumberOfSteps) {
            altLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps))));
        } else {
            altLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps)))) * -1;
        }
    }

    if (altOrAz == 2) {
        NumberOfSteps = steps * ChangeInLength * GearRatio * MotorDirection + azLeftoverSteps[machineNumber];
        if (abs(NumberOfSteps) == NumberOfSteps) {
            azLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps))));
        } else {
            azLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps)))) * -1;
        }
    }

    return NumberOfSteps;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void linearActuatorReset(int altOrAz, float MotorDirection, float LimitAngle, float GearRatio, float b, float c, linearAngle_t AcuteObtuse, float AngleAtZero)
{
    int dirMod = 1;
    if (AcuteObtuse == OBTUSE) {
        dirMod = -1;
    }
    findLimits(altOrAz, MotorDirection * dirMod, LimitAngle); //Seeks out limit switch
    float dif = leadscrewLength(b, c, AcuteObtuse, positionAfterReset(LimitAngle), AngleAtZero) - leadscrewLength(b, c, AcuteObtuse, LimitAngle, AngleAtZero);
    float NumberOfSteps = steps * dif * GearRatio * MotorDirection;
    if (altOrAz == 2) {
        moveToPosition(1, 0, (NumberOfSteps));
    } //Moves motor away from limit switch
    if (altOrAz == 1) {
        moveToPosition(1, (NumberOfSteps), 0);
    } //Moves motor away from limit switch
}
