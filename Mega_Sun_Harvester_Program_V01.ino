//**************************************************************
//  Name    : Simple Arduino Sun Tracking / Heliostat Control Program
//  Author  : Gabriel Miller
//  Website : http://www.cerebralmeltdown.com
//  Notes   : Code for controlling various types of solar
//          : machines.
//****************************************************************
#include "Functions.h"
#include "Joystick_Control_Code.h"
#include "Libraries.h"
#include "MoveMachine.h"
#include "SunCalculations.h"
#include "TargetControl.h"
#include "globals.h"

#include <Wire.h>
#include <BigNumber.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User settings located in configuration.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//BigNumber Library Settings
calculationSpeed_t calculationSpeed = SLOW_ACCURATE; // 0 for slow, 1 for fast (but less accurate)

int preTargetsUsed, midCycle;
unsigned long updateTime = 0, now = 0;

///////////////////////////////////////////////////////////
//MISC. GLOBAL VARIABLES USED THROUGHOUT THE PROGRAM
///////////////////////////////////////////////////////////
int calculationScale = 10;
float SunsAltitude, SunsAzimuth, h, delta;

int iterationsAfterReset, machineNumber;
int targetsUsed = 1;
float altManualSpeedSwap, azManualSpeedSwap, altMove, azMove;
bool FirstIterationAfterArduinoReset = false, joystickMode, justFinishedManualControl;
///////////////////////////////////////////////////////////
//END MISC. GLOBAL VARIABLES
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//MISC. ARRAYS
///////////////////////////////////////////////////////////
float PrevAltTargetsArray[numberOfMachines] = {};
float PrevAzTargetsArray[numberOfMachines] = {};
float altLeftoverSteps[numberOfMachines] = {};
float azLeftoverSteps[numberOfMachines] = {};
float MachineTargetAlt[numberOfMachines] = {};
float MachineTargetAz[numberOfMachines] = {};
float MachinesPrevAlt[numberOfMachines] = {};
float MachinesPrevAz[numberOfMachines] = {};
///////////////////////////////////////////////////////////
//END MISC. ARRAYS
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
// VOID SETUP
///////////////////////////////////////////////////////////
void setup()
{
    Wire.begin();
    Serial.begin(9600);

    // Initialise "BigNumber" library and set precision
    BigNumber::begin(calculationScale);

    ////////////////////////////////
    //TWO WIRE STEP/DIR DRIVER BOARD CODE
    pinMode(altitudeStepPin, OUTPUT);
    pinMode(altitudeDirPin, OUTPUT);
    pinMode(azimuthStepPin, OUTPUT);
    pinMode(azimuthDirPin, OUTPUT);
    pinMode(EnablePin, OUTPUT);
    ////////////////////////////////

    //Arduino pins automatically default to inputs, so this code has thus been commented out.
    //  pinMode(azLimitPin, INPUT);
    //  pinMode(altLimitPin, INPUT);
    //  pinMode(WindProtectionSwitch, INPUT);
    //  pinMode(manualModeOnOffPin, INPUT);
    //  pinMode(HeliostatToSun, INPUT);
    //  pinMode(plusOneButton, INPUT);
    //  pinMode(minusOneButton, INPUT);

    altManualSpeedSwap = altManualSpeed;
    azManualSpeedSwap = azManualSpeed;

    //MCP23017 SETUP CODE FROM http://tronixstuff.wordpress.com/tutorials > chapter 41
    // setup addressing style
    Wire.beginTransmission(0x20);
    Wire.write((byte)0x12);
    Wire.write((byte)0x20); // use table 1.4 addressing
    Wire.endTransmission();

    // set I/O pins to outputs
    Wire.beginTransmission(0x20);
    Wire.write((byte)0x00); // IODIRA register
    Wire.write((byte)0x00); // set all of bank A to outputs
    Wire.write((byte)0x00); // set all of bank B to outputs
    Wire.endTransmission();
    //END MCP23017 SETUP CODE
    for (int i = 0; i <= numberOfMachines - 1; i++) {
        MachineOff(i);
    } //Makes sure machines are off
}
///////////////////////////////////////////////////////////
//END VOID SETUP
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// VOID LOOP
///////////////////////////////////////////////////////////
void loop()
{
    int month, year, day, dayOfWeek;
    float hour, minute, second;
    byte secondRTC, minuteRTC, hourRTC, dayOfWeekRTC, dayOfMonthRTC, monthRTC, yearRTC;

    getDateDs1307(&secondRTC, &minuteRTC, &hourRTC, &dayOfWeekRTC, &dayOfMonthRTC, &monthRTC, &yearRTC);

    //The variables below can be set to accept input from devices other than an RTC.
    dayOfWeek = dayOfWeekRTC; //NOT CURRENTLY USED
    year = yearRTC + 2000;
    month = monthRTC;
    day = dayOfMonthRTC;
    hour = hourRTC;
    minute = minuteRTC;
    second = secondRTC;
    /////////////////////////////////////////////////////////////////////////////////

    if ((digitalRead(manualModeOnOffPin) != HIGH) && (joystickMode == false)) {
        if ((hour == hourReset) && (minute == 0) && (second < 10)) { //If the variable "iterationsAfterReset" is set to 0, the machines reset themselves on the limit switches
            iterationsAfterReset = 0;
        }
        now = millis();
        if ((now + updateEvery * 1000) < updateTime) {
            updateTime = now + updateEvery;
        }
    } //END if ((digitalRead(manualModeOnOffPin)!=HIGH))

    //USE THIS CODE TO RUN THE PROGRAM AT FAST SPEED. DON"T FORGET TO UNCOMMENT THE } } TOWARD THE BOTTOM
    //YOU MAY ALSO WISH TO COMMENT OUT THE "MachineOn" AND "MachineOff" CODE TO GET RID OF THE DELAY
    //  month = 6;
    //  day = 9;
    //  for (float hour=8; hour <= 21; hour++){
    ////  Serial.print("Hour ");
    ////  Serial.println(hour);
    ////  Serial.println(TotalAltSteps);
    ////  Serial.println(TotalAzSteps);
    //    for (float minute=0; minute <= 59; minute++){
    //    secondCounter=updateEvery+1;
    ////  Serial.print("Minute ");
    ////  Serial.println(minute);

    //if (digitalRead(WindProtectionSwitch)!=HIGH && (FirstIterationAfterArduinoReset==true)){
    //Updates the position of the sun, which in turn tells the machine(s) that it is time to move.
    if ((now >= updateTime) || (iterationsAfterReset == 0)) {
        updateTime = updateEvery * 1000 + millis();

        if (joystickMode == false) {
            Serial.println(" ");
            Serial.print("Time: ");
            printtime(hour, minute, second, month, day, year, dayOfWeek);
        }

        //SunPositionAlgo_LowAc::CalculateSunsPositionLowAc(month, day, hour, minute, second, timezone, latitude, longitude, SunsAltitude, SunsAzimuth, delta, h);
        findSunsAltAndAzOne(year, month, day, timezone, hour, minute, second, latitude, longitude);
        SunsAltitude = SunsAltitude + (1.02 / tan((SunsAltitude + 10.3 / (SunsAltitude + 5.11)) * PI / 180.0)) / 60.0; //Refraction Compensation: Meeus Pg. 105

        if (useNorthAsZero == true) {
            if (SunsAzimuth < 0) {
                SunsAzimuth = (SunsAzimuth + 180) * -1;
            }
            if (SunsAzimuth > 0) {
                SunsAzimuth = (SunsAzimuth - 180) * -1;
            }

            //Serial.print("Sun's Azimuth Modified for Southern Hemisphere: ");
            //Serial.println(SunsAzimuth);
        }

        if ((joystickMode == false)) {
            Serial.print("Sun's Alt: ");
            Serial.println(SunsAltitude, 3);
            Serial.print("Sun's Az: ");
            Serial.println(SunsAzimuth, 3);
            Serial.print("Number of Machines ");
            Serial.println(numberOfMachines);
        }
    } //END Update Every X seconds

    TargetControl(second, minute, hour, day, month, year, dayOfWeek); //Checks to see if the targets have been changed
    checkJoystick();

    //This code allows for manual control of the machine through the
    //Serial Monitor when the "manual switch" is turned on.
    if ((digitalRead(manualModeOnOffPin) == HIGH) && FirstIterationAfterArduinoReset == true) {
        if (joystickMode == false) { //prevents manual control through both joystick and serial at the same time
            ManualControlThroughSerial();
            updateTime = millis();
            justFinishedManualControl = true;
        }
    }
    //END of manual control code

    for (int i = 0; i <= numberOfMachines - 1; i++) {
        if (joystickMode == false) {
            machineNumber = i; //Put which machine you are trying to control here
        }

        moveMachine(PrevAltTargetsArray[machineNumber], PrevAzTargetsArray[machineNumber],
            MachineTargetAlt[machineNumber], MachineTargetAz[machineNumber],
            altAziAssy_t(pgm_read_float(&MachineSettings[machineNumber][0])),
            machineType_t(pgm_read_float(&MachineSettings[machineNumber][1])),
            float(pgm_read_float(&MachineSettings[machineNumber][2])),
            float(pgm_read_float(&MachineSettings[machineNumber][3])),
            float(pgm_read_float(&MachineSettings[machineNumber][4])),
            float(pgm_read_float(&MachineSettings[machineNumber][5])),
            float(pgm_read_float(&MachineSettings[machineNumber][6])),
            linearAngle_t(pgm_read_float(&MachineSettings[machineNumber][7])),
            float(pgm_read_float(&MachineSettings[machineNumber][8])),
            float(pgm_read_float(&MachineSettings[machineNumber][9])),
            float(pgm_read_float(&MachineSettings[machineNumber][10])),
            float(pgm_read_float(&MachineSettings[machineNumber][11])),
            float(pgm_read_float(&MachineSettings[machineNumber][12])),
            float(pgm_read_float(&MachineSettings[machineNumber][13])),
            linearAngle_t(pgm_read_float(&MachineSettings[machineNumber][14])),
            float(pgm_read_float(&MachineSettings[machineNumber][15])),
            float(pgm_read_float(&MachineSettings[machineNumber][16])),
            float(pgm_read_float(&MachineSettings[machineNumber][17])),
            float(pgm_read_float(&MachineSettings[machineNumber][18])),
            float(pgm_read_float(&MachineSettings[machineNumber][19])));

        PrevAltTargetsArray[machineNumber] = MachineTargetAlt[machineNumber];
        PrevAzTargetsArray[machineNumber] = MachineTargetAz[machineNumber];
        preTargetsUsed = targetsUsed;

        if (joystickMode == true) {
            break;
        }
    } //END for (float i=1; i <= numberOfMachines; i++)

    if (iterationsAfterReset < 3) {
        iterationsAfterReset += 1;
    }

    if (digitalRead(WindProtectionSwitch) == HIGH && (midCycle == 1)) {
        int iloop = 0;
        while (iloop < 2) {
            delay(1000);
            Serial.println("Wind Protection Mode Enabled");
            iterationsAfterReset = 0;
            midCycle = 0;
            if (digitalRead(WindProtectionSwitch) == LOW) {
                iloop = 5;
            }
        }
    }
    if (digitalRead(WindProtectionSwitch) == HIGH) {
        midCycle = midCycle + 1;
    }

    FirstIterationAfterArduinoReset = true;
    //}//END do not reset after arduino resets when Wind Protection Switch is high
    //  //}}//End for loops that run the program at fast speed

} //End Void Loop
///////////////////////////////////////////////////////////
// END VOID LOOP
///////////////////////////////////////////////////////////
