#ifndef suncalculations_h
#define suncalculations_h

#include <BigNumber.h>

void findSunsAltAndAzOne(const int &year, const int &month, const int &day, const int &timezone, const int &hour, const int &minute, const int &second, const float &latitude, const float &longitude);
void findSunsAltAndAzOne(const int &year, const int &month, const int &day, const int &timezone, const int &hour, const int &minute, const int &second, const BigNumber &latitude, const BigNumber &longitude);

#endif
