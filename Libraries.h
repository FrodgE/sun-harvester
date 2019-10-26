#ifndef libraries_h
#define libraries_h

#include <Arduino.h>


void CalculateSunsPositionLowAc(const int &month, const int &day, const float &hour, const float &minute, const float &second, const float &timezone, float latitude, float longitude, float &SunsAltitude, float &SunsAzimuth, float &delta, float &h);

// Gets the date and time from the ds1307
void getDateDs1307(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);

void printtime(const int &hour, const int &minute, const int &second, const int &month, const int &dayOfMonth, const int &year, const int &dayOfWeek);

#endif
