// BigNumber test: calculate power of E
#include "BigNumber.h"

BigNumber ePower (BigNumber x)
  {
   
  // if x > 1 halve it and square the result
  if (x > 1)
    {
    BigNumber e = ePower (x / BigNumber (2));
    return e * e;
    }  // end of x > 1

  // some big numbers
  BigNumber n = 1, e = 1, t = 1;
  
  int i = 1;
  BigNumber E;
  
  do
  { 
    E = e;
    n *= i++;  // n is i factorial
    t *= x;    // t is x^i
    e += t / n;
  } while (e != E);

  return e;
  } // end of function ePower

void setup ()
{
  Serial.begin (115200);
  Serial.println ();
  Serial.println ();
  BigNumber::begin (65);   // max around 65 on the Uno for a power of 5

  Serial.println (ePower (5));  // e  ^ 5
  
} // end of setup

void loop () { }

