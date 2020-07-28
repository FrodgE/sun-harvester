// BigNumber test: powers
#include "BigNumber.h"

void setup ()
{
  Serial.begin (115200);
  Serial.println ();
  BigNumber::begin ();  // initialize library

  Serial.println ("--- powers of 2 ---");
  
  BigNumber a = 2;

  for (int i = 1; i <= 300; i++)
  {
    Serial.print ("2^");
    Serial.print (i);
    Serial.print (" = ");
    BigNumber p = a.pow (i);
    Serial.println (p);
  }  // end of for loop

  Serial.println ("--- powers of 3 ---");
  
  a = 3;

  for (int i = 1; i <= 300; i++)
  {
    Serial.print ("3^");
    Serial.print (i);
    Serial.print (" = ");
    BigNumber p = a.pow (i);
    Serial.println (p);
  }  // end of for loop

}  // end of setup

void loop () { }
