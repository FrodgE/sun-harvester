#ifndef functions_h
#define functions_h

#include "types.h"

//This code moves the stepper motors
void moveToPosition(const bool &Accel, const long &altsteps, const long &azsteps);

//This code is used to delay between stepper motor steps
void delayInMicroseconds(const long &delayInMicrosec);

//This code resets the machine on the limit switches
void findLimits(const altAz_t &altOrAz, const int &motorDirection, const float &limitAngle);

//This code calculates the angle to move the machine to after the limit switch has been triggered.
float positionAfterReset(const float &limitAngle);

//This code calculates the angles for the heliostat (returnaltaz = 1 will return alt, 2 returns az)
void FindHeliostatAltAndAz(const float &SunsAltitude, const float &SunsAzimuth, const float &targetalt, const float &targetaz, float &machinealt, float &machineaz);

void ManualControlThroughSerial();

float eepromReadFloat(const int &address);

void eepromWriteFloat(const int &address, const float &value);

//This code turns the machine on
void MachineOn(const int &number);

//This code turns the machine off
void MachineOff(const int &number);

#endif
