#include "Functions.h"
#include "Joystick_Control_Code.h"
#include "globals.h"

#include <EEPROM.h>
#include <Wire.h>

void moveMotorWithAccel(const long &numOfSteps, const int &stepPin, const int &DirPin, const float &maxspeed, const float &Accel);
void searchForLimit(const float &limitAngle, const int &DirPin, const int &stepPin, const int &ResetSpeed, const int &LimitPin, const long &maxResetSteps, const int &motorDirection);
float to_rad(const float &angle);
float to_deg(const float &angle);
float getFloatFromSerialMonitor();
void turnMCP23017PinOn(const int &MachineNumber);
void turnMCP23017PinOff();
byte machineToByte(const int &MachineNumber);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code moves the stepper motors
void moveToPosition(const bool &Accel, const long &altsteps, const long &azsteps)
{
    //Serial.println("azsteps");
    //Serial.println(azsteps);
    //Serial.println("altsteps");
    //Serial.println(altsteps);

    if (joystickMode == true && iterationsAfterReset > 0) { //This code runs when manually controlling the machines through joystick
        joystickMoveMotors(altsteps, altitudeStepPin, altitudeDirPin, altManualSpeed, azsteps, azimuthStepPin, azimuthDirPin, azManualSpeed);
        //moveMotorWithoutAccel(azsteps, azimuthStepPin, azimuthDirPin, azManualSpeed);
        //moveMotorWithoutAccel(altsteps, altitudeStepPin, altitudeDirPin, altManualSpeed);
        return;
    }

    if (Accel == true) { //This code runs during normal operation
        moveMotorWithAccel(azsteps, azimuthStepPin, azimuthDirPin, azSpeed, azAccel);
        moveMotorWithAccel(altsteps, altitudeStepPin, altitudeDirPin, altSpeed, altAccel);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code moves the stepper motors with acceleration
void moveMotorWithAccel(const long &numOfSteps, const int &stepPin, const int &DirPin, const float &maxspeed, const float &Accel)
{
    if (abs(numOfSteps) == numOfSteps) {
        digitalWrite(DirPin, HIGH);
    } else {
        digitalWrite(DirPin, LOW);
    }
    float minMotorDelay = (1000000 / ((maxspeed * steps) / 60)) / 2;
    float halfWay = abs(long(numOfSteps / 2));
    float maxMotorDelay = ((1000000 / ((maxspeed * steps) / 60)) * 100) / 2;
    float motorDelay = maxMotorDelay;

    float motorDelayAdjust = ((maxMotorDelay - minMotorDelay) * (maxMotorDelay - minMotorDelay)) / (Accel * 1000000);
    float numberOfStepsToReachMinFromMax = long((maxMotorDelay - minMotorDelay) / motorDelayAdjust);
    if (-0.00001 < Accel && Accel < 0.00001) {
        motorDelay = minMotorDelay;
    }

    if (abs(numOfSteps) > numberOfStepsToReachMinFromMax * 2) { //This code runs if the accleration slope "flat lines"
        for (long doSteps = 1; doSteps <= abs(numOfSteps); doSteps++) {
            delayInMicroseconds(motorDelay);
            digitalWrite(stepPin, HIGH);
            delayInMicroseconds(motorDelay);
            digitalWrite(stepPin, LOW);
            if ((motorDelay > minMotorDelay) && (doSteps < halfWay)) {
                motorDelay = motorDelay - motorDelayAdjust;
            } //Accelerates the motor
            if ((motorDelay < minMotorDelay)) {
                motorDelay = minMotorDelay;
            } //Holds the motor at its max speed
            if ((doSteps > (abs(numOfSteps) - numberOfStepsToReachMinFromMax))) {
                motorDelay = motorDelay + motorDelayAdjust;
            } //Deccelerates the motor after it gets close to the end of its move
#ifdef ARDUINO_ARCH_ESP8266
            // This loop can take a long time and trigger the software watchdog.  "Feed" the dog to prevent the trigger.
            ESP.wdtFeed();
            // Calling "yield()" would be preferred but that can take ~2ms which would affect motor movement
            // yield();
#endif
        }
    } else { //This code runs if the acceleration slope is an upside down V.
        for (long doSteps = 1; doSteps <= abs(numOfSteps); doSteps++) {
            delayInMicroseconds(motorDelay);
            digitalWrite(stepPin, HIGH);
            delayInMicroseconds(motorDelay);
            digitalWrite(stepPin, LOW);
            if ((motorDelay > minMotorDelay) && (doSteps < halfWay)) {
                motorDelay = motorDelay - motorDelayAdjust;
            } //Accelerates the motor
            if ((motorDelay < minMotorDelay)) {
                motorDelay = minMotorDelay;
            } //Holds the motor at its max speed
            if (doSteps > halfWay) {
                motorDelay = motorDelay + motorDelayAdjust;
            } //Deccelerates the motor after it gets close to the end of its move
            if (motorDelay > maxMotorDelay) {
                motorDelay = maxMotorDelay;
            }
#ifdef ARDUINO_ARCH_ESP8266
            // This loop can take a long time and trigger the software watchdog.  "Feed" the dog to prevent the trigger.
            ESP.wdtFeed();
            // Calling "yield()" would be preferred but that can take ~2ms which would affect motor movement
            // yield();
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code is used to delay between stepper motor steps
void delayInMicroseconds(const long &delayInMicrosec)
{
    long t1, t2;
    t1 = micros();
    t2 = micros();
    while ((t1 + delayInMicrosec) > t2) {
        t2 = micros();
        if (t2 < t1) {
            t2 = t1 + delayInMicrosec + 1;
        } //Check if micros() rolled over
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code resets the machine on the limit switches
void findLimits(const altAz_t &altOrAz, const int &motorDirection, const float &limitAngle)
{
    if (altOrAz == ALTITUDE) {
        searchForLimit(limitAngle, altitudeDirPin, altitudeStepPin, altResetSpeed, altLimitPin, altitudeMax, motorDirection);
    }
    if (altOrAz == AZIMUTH) {
        searchForLimit(limitAngle, azimuthDirPin, azimuthStepPin, azResetSpeed, azLimitPin, azimuthMax, motorDirection);
    }
}

void searchForLimit(const float &limitAngle, const int &DirPin, const int &stepPin, const int &ResetSpeed, const int &LimitPin, const long &maxResetSteps, const int &motorDirection)
{
    long x = 0;
    int whichDir;
    if (motorDirection != 0) { //A motorDirection of 0 will skip the reset
        if (abs(limitAngle) != limitAngle) {
            whichDir = 1;
        } else {
            whichDir = -1;
        }
        if (motorDirection * whichDir != 1) {
            digitalWrite(DirPin, LOW);
        } else {
            digitalWrite(DirPin, HIGH);
        }
        float MotorDelay = (1000000 * (60 / (steps * ResetSpeed))) / 2;
        while (x < maxResetSteps) {
            digitalWrite(stepPin, HIGH);
            delayInMicroseconds(MotorDelay);
            digitalWrite(stepPin, LOW);
            delayInMicroseconds(MotorDelay);
            if (digitalRead(LimitPin) == HIGH) {
                x = maxResetSteps;
            }
            x += 1;

#ifdef ARDUINO_ARCH_ESP8266
            // This loop can take a long time and trigger the software watchdog.  "Feed" the dog to prevent the trigger.
            ESP.wdtFeed();
            // Calling "yield()" would be preferred but that can take ~2ms which would affect motor movement
            // yield();
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code calculates the angle to move the machine to after the limit switch has been triggered.
float positionAfterReset(const float &limitAngle)
{
    float endAltAndAz;
    if (abs(limitAngle) == limitAngle) {
        endAltAndAz = limitAngle - moveAwayFromLimit;
    } else {
        endAltAndAz = limitAngle + moveAwayFromLimit;
    }
    return endAltAndAz;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code calculates the angles for the heliostat (returnaltaz = 1 will return alt, 2 returns az)
void FindHeliostatAltAndAz(const float &SunsAltitude, const float &SunsAzimuth, const float &targetalt, const float &targetaz, float &machinealt, float &machineaz)
{
    float x, y, z, z1, z2, x1, x2, y1, y2, hyp, dist;

    z1 = sin(to_rad(SunsAltitude));
    hyp = cos(to_rad(SunsAltitude));
    x1 = hyp * cos(to_rad(SunsAzimuth * -1));
    y1 = hyp * sin(to_rad(SunsAzimuth * -1));

    z2 = sin(to_rad(targetalt));
    hyp = cos(to_rad(targetalt));
    x2 = hyp * cos(to_rad(targetaz * -1));
    y2 = hyp * sin(to_rad(targetaz * -1));

    x = (x1 - x2) / 2 + x2;
    y = (y1 - y2) / 2 + y2;
    z = (z1 - z2) / 2 + z2;

    dist = sqrt(x * x + y * y + z * z);
    if ((dist > -0.0001) && (dist < 0.0001)) {
        dist = 0.0001;
    }

    machinealt = to_deg(asin(z / dist));
    machineaz = to_deg(atan2(y * -1, x));
}

float to_rad(const float &angle)
{
    return angle * (PI / 180);
}

float to_deg(const float &angle)
{
    return angle * (180 / PI);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This function returns a float number typed into the serial monitor
float getFloatFromSerialMonitor()
{
    char inData[20];
    float f = 0;
    int x = 0;
    while (x < 1) {
        String str;
        // when characters arrive over the serial port...
        if (Serial.available()) {
            // wait a bit for the entire message to arrive
            delay(100);
            int i = 0;
            // read all the available characters
            while (Serial.available() > 0) {
                char inByte = Serial.read();
                str = str + inByte;
                inData[i] = inByte;
                i += 1;
                x = 2;
            }
            f = atof(inData);
            memset(inData, 0, sizeof(inData));
        }
        if (digitalRead(manualModeOnOffPin) == LOW) {
            x = 2;
        }
    } //END WHILE X<1
    return f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ManualControlThroughSerial()
{
    float newAlt, newAz, newTargetAlt, newTargetAz;

    //if ((digitalRead(PinToMoveAltInPositiveDir)!=HIGH)&&(digitalRead(PinToMoveAltInNegativeDir)!=HIGH)
    //&&(digitalRead(PinToMoveAzInPositiveDir)!=HIGH)&&(digitalRead(PinToMoveAzInNegativeDir)!=HIGH)){
    int machineNumberInput;
    if (numberOfMachines > 1) {
        Serial.println(F("Manual Mode Active:"));
        Serial.println(F("Type Machine Number:"));
        machineNumberInput = (int)getFloatFromSerialMonitor();
        Serial.println(machineNumberInput);
    } else {
        machineNumberInput = 0;
    }

    if (machineType_t(pgm_read_float(&MachineSettings[machineNumberInput][1])) == SUN_TRACKER || digitalRead(HeliostatToSun) == HIGH) { //MANUAL CONTROL OF SUN TRACKER
        Serial.println(F("Machine's Current Altitude:"));
        Serial.println(MachinesPrevAlt[machineNumberInput], 4);
        Serial.println(F("Machine's Current Azimuth:"));
        Serial.println(MachinesPrevAz[machineNumberInput], 4);
        Serial.println(F("Input Altitude:"));
        newAlt = getFloatFromSerialMonitor();
        Serial.println(newAlt, 4);
        Serial.println(F("Input Azimuth:"));
        newAz = getFloatFromSerialMonitor();
        Serial.println(newAz, 4);

        Serial.println(F("Begin Move? Yes=1, No=0"));
        int yesOrNo = getFloatFromSerialMonitor();
        Serial.println(yesOrNo);
        if (yesOrNo == 1) {
            SunsAltitude = newAlt;
            SunsAzimuth = newAz;
        }
    } //END MANUAL CONTROL OF SUN TRACKER
    else { //Manual Control Of Heliostat Targets
        Serial.println(F("Target's Current Alt:"));
        Serial.println(MachineTargetAlt[machineNumberInput], 4);
        Serial.println(F("Target's Current Az:"));
        Serial.println(MachineTargetAz[machineNumberInput], 4);
        Serial.println(F("Input Target's New Alt:"));
        newTargetAlt = getFloatFromSerialMonitor();
        Serial.println(newTargetAlt, 4);
        Serial.println(F("Input Target's New Az:"));
        newTargetAz = getFloatFromSerialMonitor();
        Serial.println(newTargetAz, 4);

        Serial.println(F("Begin Move? Yes=1, No=0"));
        int yesOrNo = getFloatFromSerialMonitor();
        Serial.println(yesOrNo);
        if (yesOrNo == 1) {
            MachineTargetAlt[machineNumberInput] = newTargetAlt;
            MachineTargetAz[machineNumberInput] = newTargetAz;
            targetsUsed = targetsUsed + 1;
        }
    } //END MANUAL CONTROL OF HELIOSTAT TARGETS

    //}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float eepromReadFloat(const int &address)
{
    union u_tag {
        byte b[4];
        float fval;
    } u;
    u.b[0] = EEPROM.read(address);
    u.b[1] = EEPROM.read(address + 1);
    u.b[2] = EEPROM.read(address + 2);
    u.b[3] = EEPROM.read(address + 3);
    return u.fval;
}

void eepromWriteFloat(const int &address, const float &value)
{
    union u_tag {
        byte b[4];
        float fval;
    } u;
    u.fval = value;

    EEPROM.write(address, u.b[0]);
    EEPROM.write(address + 1, u.b[1]);
    EEPROM.write(address + 2, u.b[2]);
    EEPROM.write(address + 3, u.b[3]);
}

//THESE FUNCTIONS WILL TURN THE PINS OF A MCP23017 ON AND OFF. THIS ALLOWS FOR THE CONTROL OF UP TO 16 MACHINES.
//EVEN MORE MACHINES COULD BE CONTROLLED BY ADDING MORE MCP23017s, BUT THIS CODE WOULD HAVE TO BE MODIFIED.
void turnMCP23017PinOn(const int &MachineNumber)
{
    Wire.beginTransmission(0x20);
    Wire.write((byte)0x12); // GPIOA
    if (MachineNumber < 8) {
        Wire.write((byte)machineToByte(MachineNumber)); // bank A
        Wire.write((byte)0); // bank B
    }
    if (MachineNumber >= 8) {
        Wire.write((byte)0); // bank A
        Wire.write((byte)machineToByte(MachineNumber - 8)); // bank B
    }
    Wire.endTransmission();
    delay(100);
}

void turnMCP23017PinOff()
{
    Wire.beginTransmission(0x20);
    Wire.write((byte)0x12); // GPIOA
    Wire.write((byte)0); // bank A
    Wire.write((byte)0); // bank B
    Wire.endTransmission();
    delay(100);
}

byte machineToByte(const int &MachineNumber)
{
    byte x = 2;
    if (MachineNumber == 0) {
        x = 1;
    }
    if (MachineNumber > 0) {
        for (int i = 1; i < MachineNumber; i++) {
            x = 2 * x;
        }
    }
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This code turns the machine on
void MachineOn(const int &number)
{
    turnMCP23017PinOn(number);
    if (enableHIGHorLOW == ACTIVE_LOW) {
        digitalWrite(EnablePin, LOW);
    } else {
        digitalWrite(EnablePin, HIGH);
    }
}

//This code turns the machine off
void MachineOff(const int &number)
{
    turnMCP23017PinOff();
    if (digitalRead(manualModeOnOffPin) != HIGH) {
        if (enableHIGHorLOW == ACTIVE_LOW) {
            digitalWrite(EnablePin, HIGH);
        } else {
            digitalWrite(EnablePin, LOW);
        }
    }
}
