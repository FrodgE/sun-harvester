#include "Libraries.h"
#include "globals.h"

#include <Wire.h>

#define DS1307_I2C_ADDRESS 0x68

byte decToBcd(const byte &val);
byte bcdToDec(const byte &val);
void setDateDs1307(const byte &second, const byte &minute, const byte &hour, const byte &dayOfWeek, const byte &dayOfMonth, const byte &month, const byte &year);

void CalculateSunsPositionLowAc(const int &month, const int &day, const float &hour, const float &minute, const float &second, const float &timezone, float latitude, float longitude, float &SunsAltitude, float &SunsAzimuth, float &delta, float &h)
{
    int n;
    float pi = 3.14159265;
    latitude = latitude * pi / 180;
    int dayNumArray[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }; //Turns the month into the number of days since the beginning of the year.
    n = dayNumArray[month - 1] + day - 1;
    h = (float(pgm_read_word_near(HourAngleArray + n)) / 1000);
    h = h + longitude + (timezone * -1 * 15);
    h = ((((hour + minute / 60 + second / 3600) - 12) * 15) + h) * pi / 180;
    float y = (((2 * pi) / 365) * (n - 1));
    delta = (0.006918 - 0.399912 * cos(y) + 0.070257 * sin(y) - 0.006758 * cos(2 * y) + 0.000907 * sin(2 * y) - 0.002697 * cos(3 * y) + 0.00148 * sin(3 * y));
    SunsAltitude = (asin(sin(latitude) * sin(delta) + cos(latitude) * cos(delta) * cos(h))) * 180 / pi;
    SunsAzimuth = (((atan2((sin(h)), ((cos(h) * sin(latitude)) - tan(delta) * cos(latitude))))) * 180 / pi);
    delta = delta * (180 / pi);
    h = h * (180 / pi);
}

/////////////////////////////////////

// Maurice Ribble
// 4-17-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code tests the DS1307 Real Time clock on the Arduino board.
// The ds1307 works in binary coded decimal or BCD.  You can look up
// bcd in google if you aren't familior with it.  There can output
// a square wave, but I don't expose that in this code.  See the
// ds1307 for it's full capabilities.

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(const byte &val)
{
    return ((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(const byte &val)
{
    return ((val / 16 * 10) + (val % 16));
}

// Stops the DS1307, but it has the side effect of setting seconds to 0
// Probably only want to use this for testing
/*void stopDs1307()
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(0x80);
  Wire.endTransmission();
}*/

// 1) Sets the date and time on the ds1307
// 2) Starts the clock
// 3) Sets hour mode to 24 hour clock
// Assumes you're passing in valid numbers
void setDateDs1307(const byte &second, // 0-59
    const byte &minute, // 0-59
    const byte &hour, // 1-23
    const byte &dayOfWeek, // 1-7
    const byte &dayOfMonth, // 1-28/29/30/31
    const byte &month, // 1-12
    const byte &year) // 0-99
{
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write((byte)0);
    Wire.write((byte)decToBcd(second)); // 0 to bit 7 starts the clock
    Wire.write((byte)decToBcd(minute));
    Wire.write((byte)decToBcd(hour)); // If you want 12 hour am/pm you need to set
        // bit 6 (also need to change readDateDs1307)
    Wire.write((byte)decToBcd(dayOfWeek));
    Wire.write((byte)decToBcd(dayOfMonth));
    Wire.write((byte)decToBcd(month));
    Wire.write((byte)decToBcd(year));
    Wire.endTransmission();
}

// Gets the date and time from the ds1307
void getDateDs1307(byte* second,
    byte* minute,
    byte* hour,
    byte* dayOfWeek,
    byte* dayOfMonth,
    byte* month,
    byte* year)
{
    // Reset the register pointer
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write((byte)0);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

    // A few of these need masks because certain bits are control bits
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

void printtime(const int &hour, const int &minute, const int &second, const int &month, const int &dayOfMonth, const int &year, const int &dayOfWeek)
{
    Serial.print(hour, DEC);
    Serial.print(":");
    Serial.print(minute, DEC);
    Serial.print(":");
    Serial.print(second, DEC);
    Serial.print("  ");

    //if (GMT_YorN!=1){
    Serial.print(month, DEC);
    Serial.print("/");
    Serial.print(dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(year, DEC);
    Serial.print("  Day_of_week:");
    Serial.println(dayOfWeek, DEC);
    //}
}
