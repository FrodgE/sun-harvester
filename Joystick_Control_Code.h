#ifndef joystick_control_code_h
#define joystick_control_code_h

void checkJoystick();

void joystickMoveMotors(const long &altsteps, const int &altitudeStepPin, const int &altitudeDirPin, const float &altManualSpeed, const long &azsteps, const int &azimuthStepPin, const int &azimuthDirPin, const float &azManualSpeed);

#endif
