#include "Joystick_Control_Code.h"
#include "Functions.h"
#include "types.h"
#include "globals.h"


void ManualControlThroughJoystick();

float UDCenter, LRCenter;

void checkJoystick()
{
    static int manualMachineNumber = 0;
    static bool joystickTriggerOnce = false;

    if (digitalRead(plusOneButton) == HIGH && digitalRead(minusOneButton) == HIGH && joystickMode == false && joystickTriggerOnce == false) {
        joystickMode = true;
        joystickTriggerOnce = true;
        justFinishedManualControl = true;
        UDCenter = analogRead(joystickUDAnalogPin);
        LRCenter = analogRead(joystickLRAnalogPin);
        MachineOn(machineNumber);
    }
    if (digitalRead(plusOneButton) == LOW && digitalRead(minusOneButton) == LOW) {
        joystickTriggerOnce = false;
    }
    if (digitalRead(plusOneButton) == HIGH && digitalRead(minusOneButton) == HIGH && joystickMode == true && joystickTriggerOnce == false) {
        joystickMode = false;
        joystickTriggerOnce = true;
    }
    if (digitalRead(plusOneButton) == HIGH) {
        manualMachineNumber = manualMachineNumber + 1;
        if (manualMachineNumber > numberOfMachines - 1) {
            manualMachineNumber = 0;
        }
        MachineOn(manualMachineNumber);
        delay(200);
    }
    if (digitalRead(minusOneButton) == HIGH) {
        manualMachineNumber = manualMachineNumber - 1;
        if (manualMachineNumber < 0) {
            manualMachineNumber = numberOfMachines - 1;
        }
        MachineOn(manualMachineNumber);
        delay(200);
    }
    if (joystickMode == true) {
        machineNumber = manualMachineNumber;
    }
    if (joystickMode == true) {
        ManualControlThroughJoystick();
    }
}

void ManualControlThroughJoystick()
{
    float UD = analogRead(joystickUDAnalogPin);
    float LR = analogRead(joystickLRAnalogPin);

    if (UD > UDCenter + 15 || UD < UDCenter - 15 || LR > LRCenter + 15 || LR < LRCenter - 15) {
        altMove = ((UD - UDCenter) / UDCenter) * 0.75 * invertUD;
        azMove = ((LR - LRCenter) / LRCenter) * 0.75 * invertLR;

        altManualSpeed = abs(altManualSpeedSwap * altMove);
        azManualSpeed = abs(azManualSpeedSwap * azMove);

        if (altManualSpeed < 5) {
            altManualSpeed = 5;
        }
        if (azManualSpeed < 5) {
            azManualSpeed = 5;
        }

        if (digitalRead(HeliostatToSun) == HIGH || machineType_t(pgm_read_float(&MachineSettings[machineNumber][1])) == SUN_TRACKER) {
            SunsAltitude = MachinesPrevAlt[machineNumber] + altMove;
            SunsAzimuth = MachinesPrevAz[machineNumber] + azMove;
        }

        if (machineType_t(pgm_read_float(&MachineSettings[machineNumber][1])) == HELIOSTAT) {
            MachineTargetAlt[machineNumber] = MachineTargetAlt[machineNumber] + altMove;
            MachineTargetAz[machineNumber] = MachineTargetAz[machineNumber] + azMove;
        }
    }
    if (digitalRead(manualModeOnOffPin) == HIGH) {
        MachineTargetAlt[machineNumber] = 0;
        MachineTargetAz[machineNumber] = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void joystickMoveMotors(long altsteps, int altitudeStepPin, int altitudeDirPin, float altManualSpeed, long azsteps, int azimuthStepPin, int azimuthDirPin, float azManualSpeed)
{
    float altMotorDelay, azMotorDelay, MotorDelay;
    long mostSteps;

    if (abs(altsteps) == altsteps) {
        digitalWrite(altitudeDirPin, HIGH);
    } else {
        digitalWrite(altitudeDirPin, LOW);
    }
    if (abs(azsteps) == azsteps) {
        digitalWrite(azimuthDirPin, HIGH);
    } else {
        digitalWrite(azimuthDirPin, LOW);
    }

    altMotorDelay = (1000000 * (60 / (steps * altManualSpeed))) / 2;
    azMotorDelay = (1000000 * (60 / (steps * azManualSpeed))) / 2;

    if (altMotorDelay < azMotorDelay) {
        MotorDelay = altMotorDelay;
    } else {
        MotorDelay = azMotorDelay;
    }
    if (abs(altsteps) > abs(azsteps)) {
        mostSteps = abs(altsteps);
    } else {
        mostSteps = abs(azsteps);
    }
    
    long x = 0;
    while (x < mostSteps) {
        if (x < abs(altsteps)) {
            digitalWrite(altitudeStepPin, HIGH);
        }
        if (x < abs(azsteps)) {
            digitalWrite(azimuthStepPin, HIGH);
        }
        delayInMicroseconds(MotorDelay);
        if (x < abs(altsteps)) {
            digitalWrite(altitudeStepPin, LOW);
        }
        if (x < abs(azsteps)) {
            digitalWrite(azimuthStepPin, LOW);
        }
        delayInMicroseconds(MotorDelay);
        x += 1;
    }
}
