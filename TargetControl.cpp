#include "TargetControl.h"
#include "Functions.h"
#include "globals.h"


void TargetControl(int second, int minute, int hour, int day, int month, int year, int dayOfWeek)
{
    if (digitalRead(manualModeOnOffPin) != HIGH && FirstIterationAfterArduinoReset == true && joystickMode == false) {
        /////////////////////////////////////////////////////////////////////////////
        //PROGRAM THE TARGETS HERE
        //////////////////////////////////////////////////////////////////////////////

        //READS POTENTIOMTER
        int analogReading = int(float(analogRead(A2)) / 10.0) * 10;
        delay(500);
        int analogReading2 = int(float(analogRead(A2)) / 10.0) * 10;
        while (abs(analogReading - analogReading2) > 35) {
            delay(500);
            analogReading = analogRead(A2);
            delay(500);
            analogReading2 = analogRead(A2);
        }

        //CHANGES TARGET GROUP BASED ON POTENTIOMETER READING
        if (analogReading > 0) {
            targetsUsed = 1;
        }
        if (analogReading > 200) {
            targetsUsed = 2;
        }
        if (analogReading > 400) {
            targetsUsed = 3;
        }
        if (analogReading > 600) {
            targetsUsed = 4;
        }
        if (analogReading > 800) {
            targetsUsed = 5;
        }
        if (potentiometerIsConnected == false) {
            targetsUsed = 1;
        }

        //SAVES TARGET SETTINGS WHEN MANUAL CONTROL THROUGH EITHER THE SERIAL MONITOR OR THE JOYSTICK IS TURNED OFF
        if (justFinishedManualControl == true && digitalRead(manualModeOnOffPin) != HIGH) {
            Serial.println("Writing to EPPROM");
            for (int i = 0; i <= numberOfMachines - 1; i++) {
                eepromWriteFloat(i * 8 + 2 * 16 * targetsUsed - 1, MachineTargetAlt[i]);
                eepromWriteFloat(i * 8 + 4 + 2 * 16 * targetsUsed - 1, MachineTargetAz[i]);
            }
            justFinishedManualControl = false;
        }

        //READS TARGET SETTINGS
        for (int i = 0; i <= numberOfMachines - 1; i++) {
            MachineTargetAlt[i] = eepromReadFloat(i * 8 + 2 * 16 * targetsUsed - 1);
            MachineTargetAz[i] = eepromReadFloat(i * 8 + 4 + 2 * 16 * targetsUsed - 1);
        }

        //MachineTargetAlt[0] = 0;
        //MachineTargetAz[0]  = 0;

        /////////////////////////////////////////////////////////////////////////////
        //END PROGRAM THE TARGETS
        //////////////////////////////////////////////////////////////////////////////

    } //ENDif (digitalRead(manualModeOnOffPin)!=HIGH && FirstIterationAfterArduinoReset==true)
}
