#ifndef libraries_h
#define libraries_h

#include <Arduino.h>


void CalculateSunsPositionLowAc(int month, int day, float hour, float minute, float second, float timezone, float latitude, float longitude, float &SunsAltitude, float &SunsAzimuth, float &delta, float &h);

// Gets the date and time from the ds1307
void getDateDs1307(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);

void printtime(int hour, int minute, int second, int month, int dayOfMonth, int year, int dayOfWeek);

#endif
