#ifndef functions_h
#define functions_h

//This code moves the stepper motors
void moveToPosition(int AccelYesOrNo, long altsteps, long azsteps);

//This code is used to delay between stepper motor steps
void delayInMicroseconds(long delayInMicrosec);

//This code resets the machine on the limit switches
void findLimits(int altOrAz, int motorDirection, float limitAngle);

//This code calculates the angle to move the machine to after the limit switch has been triggered.
float positionAfterReset(float limitAngle);

//This code calculates the angles for the heliostat (returnaltaz = 1 will return alt, 2 returns az)
void FindHeliostatAltAndAz(float SunsAltitude, float SunsAzimuth, float targetalt, float targetaz, float &machinealt, float &machineaz);

void ManualControlThroughSerial();

float eepromReadFloat(int address);

void eepromWriteFloat(int address, float value);

//This code turns the machine on
void MachineOn(int number);

//This code turns the machine off
void MachineOff(int number);

#endif
