#ifndef joystick_control_code_h
#define joystick_control_code_h

void checkJoystick();

void joystickMoveMotors(long altsteps, int altStepPin, int altDirPin, float altManualSpd, long azsteps, int azimuthStepPin, int azimuthDirPin, float azManualSpeed);

#endif
