#include "Libraries.h"
#include "globals.h"

#include <Wire.h>

#define DS1307_I2C_ADDRESS 0x68

byte decToBcd(const byte &val);
byte bcdToDec(const byte &val);
void setDateDs1307(const byte &second, const byte &minute, const byte &hour, const byte &dayOfWeek, const byte &dayOfMonth, const byte &month, const byte &year);

///////////////////////////////////////////////////////////////
//HOUR ANGLE TABLE FOR LOW ACCURACY SUN POSITION CALCULATIONS
///////////////////////////////////////////////////////////////
const PROGMEM int HourAngleArray[] = {
    -601, -721, -840, -957, -1073, -1188, -1301, -1412, -1521, -1629, -1734, -1837, -1937, -2035, -2131, -2224, -2314, -2402, -2486, -2568, -2647, -2723, -2795, -2865, -2931, -2994, -3054, -3110, -3163, -3213, -3259, -3302, -3342, -3378, -3411, -3440, -3466, -3489, -3508, -3524, -3537, -3546, -3552, -3555, -3554, -3551, -3544, -3534, -3522, -3506,
    -3487, -3466, -3441, -3414, -3384, -3352, -3317, -3279, -3240, -3197, -3153, -3106, -3057, -3006, -2954, -2899, -2842, -2784, -2724, -2662, -2599, -2535, -2469, -2402, -2334, -2264, -2194, -2123, -2051, -1978, -1905, -1831, -1756, -1681, -1606, -1531, -1455, -1380, -1304, -1229, -1154, -1079, -1004, -930, -856, -783, -711, -639, -568, -498,
    -429, -361, -295, -229, -165, -102, -40, 19, 78, 135, 191, 244, 296, 347, 395, 441, 486, 528, 569, 607, 643, 677, 709, 739, 766, 791, 814, 835, 853, 868, 882, 893, 901, 908, 912, 913, 912, 909, 903, 895, 885, 873, 858, 841, 822, 800, 777, 751, 724, 694,
    663, 629, 594, 557, 519, 479, 437, 394, 350, 304, 257, 209, 160, 110, 59, 7, -44, -97, -151, -205, -259, -313, -368, -422, -477, -531, -585, -638, -691, -744, -795, -846, -896, -946, -994, -1040, -1086, -1131, -1173, -1215, -1255, -1293, -1330, -1365, -1398, -1429, -1458, -1485, -1510, -1533,
    -1553, -1572, -1588, -1602, -1613, -1622, -1629, -1633, -1635, -1634, -1631, -1625, -1617, -1606, -1592, -1577, -1558, -1537, -1514, -1488, -1459, -1428, -1395, -1359, -1321, -1281, -1238, -1193, -1145, -1096, -1044, -990, -934, -876, -815, -753, -689, -624, -556, -487, -416, -343, -269, -194, -117, -39, 40, 121, 202, 285,
    369, 454, 539, 625, 712, 800, 888, 976, 1065, 1154, 1243, 1332, 1422, 1511, 1600, 1689, 1777, 1865, 1953, 2040, 2126, 2212, 2296, 2380, 2463, 2545, 2625, 2704, 2782, 2859, 2934, 3007, 3079, 3149, 3217, 3283, 3347, 3409, 3469, 3526, 3582, 3635, 3685, 3733, 3779, 3821, 3861, 3898, 3933, 3964,
    3993, 4018, 4040, 4060, 4076, 4088, 4098, 4104, 4107, 4106, 4102, 4095, 4084, 4069, 4052, 4030, 4005, 3977, 3945, 3909, 3870, 3827, 3781, 3731, 3678, 3622, 3562, 3499, 3432, 3362, 3289, 3213, 3134, 3051, 2966, 2878, 2787, 2693, 2597, 2498, 2397, 2293, 2187, 2079, 1969, 1858, 1744, 1629, 1512, 1394,
    1275, 1154, 1033, 910, 787, 664, 540, 416, 291, 167, 43, -80, -204, -326, -449
};
///////////////////////////////////////////////////////////////
//END HOUR ANGLE TABLE FOR LOW ACCURACY SUN POSITION CALCULATIONS
///////////////////////////////////////////////////////////////

void CalculateSunsPositionLowAc(const int &month, const int &day, const float &hour, const float &minute, const float &second, const float &timezone, float latitude, float longitude, float &SunsAltitude, float &SunsAzimuth, float &delta, float &h)
{
    int n;
    latitude = latitude * PI / 180;
    int dayNumArray[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }; //Turns the month into the number of days since the beginning of the year.
    n = dayNumArray[month - 1] + day - 1;
    h = (float(pgm_read_word_near(HourAngleArray + n)) / 1000);
    h = h + longitude + (timezone * -1 * 15);
    h = ((((hour + minute / 60 + second / 3600) - 12) * 15) + h) * PI / 180;
    float y = (((2 * PI) / 365) * (n - 1));
    delta = (0.006918 - 0.399912 * cos(y) + 0.070257 * sin(y) - 0.006758 * cos(2 * y) + 0.000907 * sin(2 * y) - 0.002697 * cos(3 * y) + 0.00148 * sin(3 * y));
    SunsAltitude = (asin(sin(latitude) * sin(delta) + cos(latitude) * cos(delta) * cos(h))) * 180 / PI;
    SunsAzimuth = (((atan2((sin(h)), ((cos(h) * sin(latitude)) - tan(delta) * cos(latitude))))) * 180 / PI);
    delta = delta * (180 / PI);
    h = h * (180 / PI);
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
