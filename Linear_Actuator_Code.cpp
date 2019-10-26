#include "Linear_Actuator_Code.h"
#include "Functions.h"
#include "globals.h"

#include <Arduino.h>

float leadscrewLength(const float &b, const float &c, const linearAngle_t &AcuteObtuse, float angle, const float &AngleAtZero);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code calculates the length of the linear actuator.
float leadscrewLength(const float &b, const float &c, const linearAngle_t &AcuteObtuse, float angle, const float &AngleAtZero)
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
long linearActuatorMoveMotor(const altAz_t &altOrAz, const float &MachinesPreviousAngle, const float &MachinesNewAngle, const float &GearRatio, const float &MotorDirection, const float &b, const float &c, const linearAngle_t &AcuteObtuse, const float &AngleAtZero)
{
    float NewLength = leadscrewLength(b, c, AcuteObtuse, MachinesNewAngle, AngleAtZero); //New Leadscrew Length
    float PreviousLength = leadscrewLength(b, c, AcuteObtuse, MachinesPreviousAngle, AngleAtZero); //Previous Leadscrew Length
    float ChangeInLength = NewLength - PreviousLength;
    float NumberOfSteps;

    if (altOrAz == ALTITUDE) {
        NumberOfSteps = steps * ChangeInLength * GearRatio * MotorDirection + altLeftoverSteps[machineNumber];
        if (abs(NumberOfSteps) == NumberOfSteps) {
            altLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps))));
        } else {
            altLeftoverSteps[machineNumber] = abs(float(NumberOfSteps - float(long(NumberOfSteps)))) * -1;
        }
    }

    if (altOrAz == AZIMUTH) {
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
void linearActuatorReset(const altAz_t &altOrAz, const float &MotorDirection, const float &LimitAngle, const float &GearRatio, const float &b, const float &c, const linearAngle_t &AcuteObtuse, const float &AngleAtZero)
{
    int dirMod = 1;
    if (AcuteObtuse == OBTUSE) {
        dirMod = -1;
    }
    findLimits(altOrAz, MotorDirection * dirMod, LimitAngle); //Seeks out limit switch
    float dif = leadscrewLength(b, c, AcuteObtuse, positionAfterReset(LimitAngle), AngleAtZero) - leadscrewLength(b, c, AcuteObtuse, LimitAngle, AngleAtZero);
    float NumberOfSteps = steps * dif * GearRatio * MotorDirection;
    if (altOrAz == AZIMUTH) {
        moveToPosition(true, 0, (NumberOfSteps));
    } //Moves motor away from limit switch
    if (altOrAz == ALTITUDE) {
        moveToPosition(true, (NumberOfSteps), 0);
    } //Moves motor away from limit switch
}
