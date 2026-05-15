#include "hal_math.h"

/*******************
Integer amplitude limiting function
********************/
int Xianfu_int(int value,int Amplitude)
{
	int temp;
	if(value>Amplitude) temp = Amplitude;
	else if(value<0) temp = 0;
	else temp = value;
	return temp;
}
int Xianfu_float(float value,float Amplitude)
{
	int temp;
	if(value>Amplitude) temp = Amplitude;
	else if(value<-Amplitude) temp = -Amplitude;
	else temp = value;
	return temp;
}