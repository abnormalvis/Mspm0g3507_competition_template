#include "Servo.h"

/* No PWM_Servo peripheral configured in syscfg yet.
 * Stubs preserve API for track_position_ctrl.c; add a TIMG PWM block
 * (e.g. CCP0/CCP1 on PA17/PA12) to syscfg, then enable the original code. */

void Servo_setAngle1(float Angle){
	if(Angle>=SERVO1_MAX_ANGLE){ Angle=SERVO1_MAX_ANGLE; }
	else if(Angle<=SERVO1_MIN_ANGLE){ Angle=SERVO1_MIN_ANGLE; }
	(void)Angle;
}

void Servo_setAngle2(float Angle){
	if(Angle>=SERVO2_MAX_ANGLE){ Angle=SERVO2_MAX_ANGLE; }
	else if(Angle<=SERVO2_MIN_ANGLE){ Angle=SERVO2_MIN_ANGLE; }
	(void)Angle;
}
