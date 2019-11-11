#include "configuration.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//PUT YOUR LATITUDE, LONGITUDE, AND TIME ZONE HERE
// Use strings so the "BigNumber" library can be used to avoid loss of precision
const char latitude[] = "39.19";
const char longitude[] = "-78.16";
const float timezone = -5;

//If you live in the northern hemisphere, put false here. If you live in the southern hemisphere put true.
const bool useNorthAsZero = false;

//INTERVAL BETWEEN MACHINE POSITION UPDATES
//Lets the program know how often to update the position of the machine(s) (in seconds)
const unsigned long updateEvery = 30; //seconds

//TIME MACHINE(S) RESET
//Put the hour you want your solar machines to reset on
//their limit switches here. Remember that daylight saving time is not used, so the
//hour it resets won't necessarily match the clock in your home.
const int hourReset = 21;

//DEGREES TO MOVE OFF OF LIMIT SWITCHES
//Tells the program how many degrees to move off of limit switches after they have been triggered.
//Choose a positive number.
const float moveAwayFromLimit = 5;

//Machine Wind Protection Park Angle
const float machineAltParkAngle = 65;
const float machineAzParkAngle = 0;

//Put false here if you have not connected the target changer potentiometer to the circuit. Put true here if it is connected.
const bool potentiometerIsConnected = true;

// STEPPER MOTOR SETUP
//The number of steps required for your stepper to make one revolution. (Don't forget to take into
//account the settings on your driver board. i.e. Microstepping, half stepping etc.)
const float steps = 800;
//const float steps = 1600;
//Set the travel speed for your stepper motors here (Roughly in Rev/Min)
const float altSpeed = 40;
const float azSpeed = 40;
//Set the speed the motors travel when they reset here (Roughly in Rev/Min)
//Note: Max reset speed is limited since checking to see if the limit has been triggered slows things down.
//The result of which means that the alt/az reset speeds may seem noticably slower than normal travel speed.
const float altResetSpeed = 40;
const float azResetSpeed = 40;
//Speed of Stepper Motors when using manual control through joystick
float altManualSpeed = 40;
float azManualSpeed = 40;
//enable on HIGH or LOW
//Put a 0 here if your driver boards enable the stepper motors when the enable pin is written LOW.
//Put a 1 here if your driver boards enable the stepper motors when the enable pin is written HIGH.
const activeHighLow_t enableHIGHorLOW = ACTIVE_LOW;

//Number of seconds (roughly) to accelerate to full speed.
const float altAccel = 1;
const float azAccel = 1;
//the maximum number of steps the machine(s) will take when resetting
const long altitudeMax = 1000000;
const long azimuthMax = 1000000;

///////////////////////////////////////////////////////////
//PIN ASSIGNMENT
///////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//TWO WIRE STEP/DIR DRIVER BOARD Pin Assignment
const int azimuthStepPin = 2;
const int azimuthDirPin = 3;

const int altitudeStepPin = 4;
const int altitudeDirPin = 5;

const int EnablePin = 6;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Limit Switch Pin Assignment
const int altLimitPin = 7;
const int azLimitPin = 7;

//Manual Control Pin Assignment
const int manualModeOnOffPin = 8;
//Wind Protection Switch Pin Assignment
const int WindProtectionSwitch = 10;
//Heliostat to Sun Tracker Toggle Switch pin
const int HeliostatToSun = 9;

//Joystick Up Down and Left Right pins.
const int joystickUDAnalogPin = A0;
const int joystickLRAnalogPin = A1;
const int invertUD = -1; //Change value from 1 to -1 to invert up/down during joystick control
const int invertLR = -1; //Change value from 1 to -1 to invert left/right during joystick control

//Select machine to manually control through joystick button pins
const int plusOneButton = 11;
const int minusOneButton = 12;


//Open the "ReadMe" file that was downloaded with this program to see what each of these settings do.
//Example Machine That uses Gear Reduction System for both AZIMUTH and ALTITUDE
#define SETTING_0_GEAR_OR_LINEAR            ALT_GEAR_AZI_GEAR
#define SETTING_1_SUN_TRACKER_OR_HELIOSTAT  HELIOSTAT
#define SETTING_2_ALT_RATIO                 150
#define SETTING_3_ALT_DIRECTION             -1
#define SETTING_4_ALT_LINEAR_LENGTH_A       0
#define SETTING_5_ALT_LINEAR_LENGTH_B       0
#define SETTING_6_ALT_ANGLE_AT_ZERO         0
#define SETTING_7_ALT_LINEAR_POS_AC_OB      NOT_USED
#define SETTING_8_ALT_HOME_SWITCH           -1
#define SETTING_9_AZI_GEAR_RATIO            50
#define SETTING_10_AZI_DIRECTION            -1
#define SETTING_11_AZI_LINEAR_LENGTH_A      0
#define SETTING_12_AZI_LINEAR_LENGTH_B      0
#define SETTING_13_AZI_ANGLE_AT_ZERO        0
#define SETTING_14_AZI_LINEAR_POS_AC_OB     NOT_USED
#define SETTING_15_AZI_HOME_SWITCH          -179
#define SETTING_16_AZI_MINIMUM_ANGLE        -179
#define SETTING_17_ALT_MINIMUM_ANGLE        -1
#define SETTING_18_AZI_MAXIMUM_ANGLE        179
#define SETTING_19_ALT_MAXIMUM_ANGLE        110

//Example Machine That uses Linear Actuator for both AZIMUTH and ALTITUDE
//#define SETTING_0_GEAR_OR_LINEAR            ALT_LINEAR_AZI_LINEAR
//#define SETTING_1_SUN_TRACKER_OR_HELIOSTAT  HELIOSTAT
//#define SETTING_2_ALT_RATIO                 14
//#define SETTING_3_ALT_DIRECTION             1
//#define SETTING_4_ALT_LINEAR_LENGTH_A       8.21
//#define SETTING_5_ALT_LINEAR_LENGTH_B       8.875
//#define SETTING_6_ALT_ANGLE_AT_ZERO         78.41
//#define SETTING_7_ALT_LINEAR_POS_AC_OB      ACUTE
//#define SETTING_8_ALT_HOME_SWITCH           -1.5
//#define SETTING_9_AZI_GEAR_RATIO            14
//#define SETTING_10_AZI_DIRECTION            -1
//#define SETTING_11_AZI_LINEAR_LENGTH_A      5.5
//#define SETTING_12_AZI_LINEAR_LENGTH_B      5.5
//#define SETTING_13_AZI_ANGLE_AT_ZERO        90
//#define SETTING_14_AZI_LINEAR_POS_AC_OB     OBTUSE
//#define SETTING_15_AZI_HOME_SWITCH          -58.839
//#define SETTING_16_AZI_MINIMUM_ANGLE        -58.839
//#define SETTING_17_ALT_MINIMUM_ANGLE        -1.5
//#define SETTING_18_AZI_MAXIMUM_ANGLE        65
//#define SETTING_19_ALT_MAXIMUM_ANGLE        66

//Example Machine That uses Linear Actuator for ALTITUDE and Gear Reduction System for AZIMUTH
//#define SETTING_0_GEAR_OR_LINEAR            ALT_LINEAR_AZI_GEAR
//#define SETTING_1_SUN_TRACKER_OR_HELIOSTAT  SUN_TRACKER
//#define SETTING_2_ALT_RATIO                 2
//#define SETTING_3_ALT_DIRECTION             -1
//#define SETTING_4_ALT_LINEAR_LENGTH_A       956
//#define SETTING_5_ALT_LINEAR_LENGTH_B       1016
//#define SETTING_6_ALT_ANGLE_AT_ZERO         90
//#define SETTING_7_ALT_LINEAR_POS_AC_OB      ACUTE
//#define SETTING_8_ALT_HOME_SWITCH           -1
//#define SETTING_9_AZI_GEAR_RATIO            5067
//#define SETTING_10_AZI_DIRECTION            -1
//#define SETTING_11_AZI_LINEAR_LENGTH_A      0
//#define SETTING_12_AZI_LINEAR_LENGTH_B      0
//#define SETTING_13_AZI_ANGLE_AT_ZERO        0
//#define SETTING_14_AZI_LINEAR_POS_AC_OB     NOT_USED
//#define SETTING_15_AZI_HOME_SWITCH          -94
//#define SETTING_16_AZI_MINIMUM_ANGLE        -94
//#define SETTING_17_ALT_MINIMUM_ANGLE        -1
//#define SETTING_18_AZI_MAXIMUM_ANGLE        120
//#define SETTING_19_ALT_MAXIMUM_ANGLE        88

//Example Machine That uses Linear Actuator for AZIMUTH and Gear Reduction System for ALTITUDE
//#define SETTING_0_GEAR_OR_LINEAR            ALT_GEAR_AZI_LINEAR
//#define SETTING_1_SUN_TRACKER_OR_HELIOSTAT  SUN_TRACKER
//#define SETTING_2_ALT_RATIO                 36
//#define SETTING_3_ALT_DIRECTION             1
//#define SETTING_4_ALT_LINEAR_LENGTH_A       0
//#define SETTING_5_ALT_LINEAR_LENGTH_B       0
//#define SETTING_6_ALT_ANGLE_AT_ZERO         0
//#define SETTING_7_ALT_LINEAR_POS_AC_OB      NOT_USED
//#define SETTING_8_ALT_HOME_SWITCH           -1
//#define SETTING_9_AZI_GEAR_RATIO            20
//#define SETTING_10_AZI_DIRECTION            1
//#define SETTING_11_AZI_LINEAR_LENGTH_A      16
//#define SETTING_12_AZI_LINEAR_LENGTH_B      11
//#define SETTING_13_AZI_ANGLE_AT_ZERO        90
//#define SETTING_14_AZI_LINEAR_POS_AC_OB     ACUTE
//#define SETTING_15_AZI_HOME_SWITCH          -60
//#define SETTING_16_AZI_MINIMUM_ANGLE        -60
//#define SETTING_17_ALT_MINIMUM_ANGLE        -1
//#define SETTING_18_AZI_MAXIMUM_ANGLE        60
//#define SETTING_19_ALT_MAXIMUM_ANGLE        160

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//END OF USER SETTINGS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const float MachineSettings[][20] PROGMEM = {
                                            SETTING_0_GEAR_OR_LINEAR,
                                            SETTING_1_SUN_TRACKER_OR_HELIOSTAT,
                                            SETTING_2_ALT_RATIO,
                                            SETTING_3_ALT_DIRECTION,
                                            SETTING_4_ALT_LINEAR_LENGTH_A,
                                            SETTING_5_ALT_LINEAR_LENGTH_B,
                                            SETTING_6_ALT_ANGLE_AT_ZERO,
                                            SETTING_7_ALT_LINEAR_POS_AC_OB,
                                            SETTING_8_ALT_HOME_SWITCH,
                                            SETTING_9_AZI_GEAR_RATIO,
                                            SETTING_10_AZI_DIRECTION,
                                            SETTING_11_AZI_LINEAR_LENGTH_A,
                                            SETTING_12_AZI_LINEAR_LENGTH_B,
                                            SETTING_13_AZI_ANGLE_AT_ZERO,
                                            SETTING_14_AZI_LINEAR_POS_AC_OB,
                                            SETTING_15_AZI_HOME_SWITCH,
                                            SETTING_16_AZI_MINIMUM_ANGLE,
                                            SETTING_17_ALT_MINIMUM_ANGLE,
                                            SETTING_18_AZI_MAXIMUM_ANGLE,
                                            SETTING_19_ALT_MAXIMUM_ANGLE
};
